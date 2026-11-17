#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <duktape.h>
#include <Arduino.h>

#include "utilities.h"
#include "glue.h"

#include <stdint.h>
#include <stddef.h>

/*
   This version of javascript does not seem to understand:

   let
   () = { ... }
   
*/
   

extern const char app_js_start[] asm("_binary_data_app_js_start");
extern const char app_js_end[]   asm("_binary_data_app_js_end");

size_t js_size = app_js_end - app_js_start;

static duk_context *ctx;
static uint8_t *screen;
static int sx, sy;

static char input[2048];
static size_t input_pos;

/* Screen access */

static duk_ret_t js_get_pixel(duk_context *ctx)
{
    int x = duk_require_int(ctx, 0);
    int y = duk_require_int(ctx, 1);

    if (x < 0 || x >= sx || y < 0 || y >= sy)
        return duk_error(ctx, DUK_ERR_RANGE_ERROR,
                         "pixel coordinates out of bounds");

    // FIXME
    int i = y * sx + x;
    duk_push_int(ctx, (screen[i / 8] & (0x80U >> (i % 8))) != 0);
    return 1;
}

static duk_ret_t js_set_pixel(duk_context *ctx)
{
    int x = duk_require_int(ctx, 0);
    int y = duk_require_int(ctx, 1);
    int value = duk_require_int(ctx, 2);

    if (x < 0 || x >= sx || y < 0 || y >= sy)
      return 1;
    /*
      We want to avoid drawing out of bounds, for compatibility with Bangle. 
        return duk_error(ctx, DUK_ERR_RANGE_ERROR,
                         "pixel coordinates out of bounds");
			*/
    if (value < 0 || value > 255)
        return duk_error(ctx, DUK_ERR_RANGE_ERROR,
                         "pixel value must be between 0 and 255");

    int i = y * sx + x;
    uint8_t mask = (uint8_t)(0x80U >> (i % 8));

    /* Preserve the original implementation's pixel polarity. */
    if (value == 0)
        screen[i / 8] |= mask;
    else
        screen[i / 8] &= (uint8_t)~mask;

    return 0;
}

static duk_ret_t js_print(duk_context *ctx)
{
    duk_idx_t count = duk_get_top(ctx);

    for (duk_idx_t i = 0; i < count; ++i) {
        if (i != 0)
            Serial.print(' ');

        Serial.print(duk_safe_to_string(ctx, i));
    }
    Serial.println();

    return 0;
}

static duk_ret_t js_cprint(duk_context *ctx)
{
    const char *text = duk_safe_to_string(ctx, 0);
    Serial.println(text);
    return 0;
}

static void js_register_globals(void)
{
    duk_push_c_function(ctx, js_print, DUK_VARARGS);
    duk_put_global_string(ctx, "print");

    duk_push_c_function(ctx, js_cprint, 1);
    duk_put_global_string(ctx, "__cprint");

    duk_push_c_function(ctx, js_get_pixel, 2);
    duk_put_global_string(ctx, "__get_pixel");

    duk_push_c_function(ctx, js_set_pixel, 3);
    duk_put_global_string(ctx, "__set_pixel");

    duk_push_int(ctx, sx);
    duk_put_global_string(ctx, "__screen_width");

    duk_push_int(ctx, sy);
    duk_put_global_string(ctx, "__screen_height");
}

static int js_eval(const char *code)
{
    if (duk_peval_string(ctx, code) != 0) {
        Serial.print("JavaScript error: ");
        Serial.println(duk_safe_to_string(ctx, -1));
        duk_pop(ctx);
        return -1;
    }

    duk_pop(ctx);
    return 0;
}

int js_init(uint8_t *_screen, int _sx, int _sy)
{
    if (!_screen || _sx <= 0 || _sy <= 0)
        return -1;

    screen = _screen;
    sx = _sx;
    sy = _sy;

    ctx = duk_create_heap_default();
    if (!ctx)
        return -1;

    js_register_globals();

    if (js_eval(app_js_start) != 0)
        return -1;

    js_eval("print('js: Scripting initialized.');");
    return 0;
}

static int js_call(const char *name, int nargs, int *args, int *result)
{
    if (!ctx)
        return -1;

    duk_get_global_string(ctx, name);
    if (!duk_is_function(ctx, -1)) {
        duk_pop(ctx);
        Serial.print("JavaScript function not found: ");
        Serial.println(name);
        return -1;
    }

    for (int i = 0; i < nargs; ++i)
        duk_push_int(ctx, args[i]);

    if (duk_pcall(ctx, nargs) != 0) {
        Serial.print("JavaScript error: ");
        Serial.println(duk_safe_to_string(ctx, -1));
        duk_pop(ctx);
        return -1;
    }

    *result = duk_to_int(ctx, -1);
    duk_pop(ctx);
    return 0;
}

static int __js_exec_input(void)
{
    input[input_pos] = '\0';
    Serial.printf("\n# exec, %u chars\n", (unsigned)input_pos);

    int rc = js_eval(input);
    if (rc == 0)
        Serial.println("# done");

    return rc;
}

int js_exec_input(void)
{
    return __js_exec_input();
}

/* Returns 1 if the screen should be redrawn. */
int js_step(void)
{
    if (!ctx)
        return -1;

    while (Serial.available()) {
        int c = Serial.read();

        if (c == 2) { /* Ctrl-B executes the accumulated input. */
            if (input_pos < sizeof(input))
                __js_exec_input();
            else
                Serial.println("Input buffer full");

            input_pos = 0;
            continue;
        }

        Serial.write((char)c);

        if (input_pos < sizeof(input) - 1) {
            input[input_pos++] = (char)c;
        } else {
            Serial.println("\nInput buffer full; input discarded");
            input_pos = 0;
        }
    }

    int args[] = { 666 };
    int result = 0;

    if (js_call("__step", 1, args, &result) != 0)
        return -1;

    return result;
}

int js_touch(int down, int x, int y)
{
    int args[] = { down, x, y };
    int result = 0;

    if (js_call("__touch", 3, args, &result) != 0)
        return -1;

    return result;
}

int js_key(int press, int v)
{
    int args[] = { press, v };
    int result = 0;

    if (js_call("__key", 2, args, &result) != 0)
        return -1;

    return result;
}

void js_done(void)
{
    if (ctx) {
        duk_destroy_heap(ctx);
        ctx = NULL;
    }

    screen = NULL;
    sx = sy = 0;
    input_pos = 0;
}
