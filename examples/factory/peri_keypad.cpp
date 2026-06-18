
#include <Adafruit_TCA8418.h>
#include "utilities.h"
#include "peripheral.h"

#define KEYPAD_ROWS 4
#define KEYPAD_COLS 10
#define KEYPAD_PRESS_VAL_MIN   129
#define KEYPAD_PRESS_VAL_MAX   163
#define KEYPAD_RELEASE_VAL_MIN 1
#define KEYPAD_RELEASE_VAL_MAX 35

const char keymap[KEYPAD_ROWS][KEYPAD_COLS] = {
    {'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p'},
    {'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', KEYPAD_KEY_DEL},
    {KEYPAD_KEY_ALT, 'z', 'x', 'c', 'v', 'b', 'n', 'm', '$', KEYPAD_KEY_ENT},
    {KEYPAD_KEY_NONE, KEYPAD_KEY_NONE, KEYPAD_KEY_NONE, KEYPAD_KEY_NONE, KEYPAD_KEY_NONE, KEYPAD_KEY_UP, '0', KEYPAD_KEY_SPACE, KEYPAD_KEY_SYM, KEYPAD_KEY_UP},
};

#define KEYPAD_EVENT_QUEUE_LEN 16

typedef struct {
    char value;
    int state;
} keypad_event_t;

Adafruit_TCA8418 keypad; 
keypad_cb keypad_listener = NULL;
char keypad_curr_val = ' ';
int keypad_state = KEYPAD_RELEASE;
bool keypad_update = false;
static keypad_event_t keypad_event_queue[KEYPAD_EVENT_QUEUE_LEN];
static uint8_t keypad_event_head = 0;
static uint8_t keypad_event_tail = 0;

static void keypad_push_event(char value, int state)
{
    uint8_t next_tail = (uint8_t)((keypad_event_tail + 1U) % KEYPAD_EVENT_QUEUE_LEN);
    if (next_tail == keypad_event_head) {
        keypad_event_head = (uint8_t)((keypad_event_head + 1U) % KEYPAD_EVENT_QUEUE_LEN);
    }

    keypad_event_queue[keypad_event_tail].value = value;
    keypad_event_queue[keypad_event_tail].state = state;
    keypad_event_tail = next_tail;
}

static bool keypad_pop_event(char *value, int *state)
{
    if (keypad_event_head == keypad_event_tail) {
        return false;
    }

    if (value) {
        *value = keypad_event_queue[keypad_event_head].value;
    }
    if (state) {
        *state = keypad_event_queue[keypad_event_head].state;
    }

    keypad_event_head = (uint8_t)((keypad_event_head + 1U) % KEYPAD_EVENT_QUEUE_LEN);
    return true;
}

bool keypad_init(int address)
{
    if(!i2cIsInit(0)){
        Wire.begin(BOARD_KEYBOARD_SDA, BOARD_KEYBOARD_SCL);
        Wire.beginTransmission(address);
        Wire.endTransmission(true);
    }

    if (!keypad.begin(address, &Wire)) {
        // Serial.println("keypad not found, check wiring & pullups!");
        log_e("keypad not found, check wiring & pullups!");
        return false;
    }

    // configure the size of the keypad matrix.
    // all other pins will be inputs
    keypad.matrix(KEYPAD_ROWS, KEYPAD_COLS);

    // flush the internal buffer
    keypad.flush();

    return true;
}

int keypad_get_val(char *c)
{
    char value = KEYPAD_KEY_NONE;
    int state = KEYPAD_RELEASE;

    while (keypad_pop_event(&value, &state)) {
        if (state == KEYPAD_PRESS) {
            if (c) {
                *c = value;
            }
            keypad_curr_val = value;
            keypad_state = state;
            return true;
        }
    }

    return false;
}

int keypad_get_event(char *c, int *state)
{
    char value = KEYPAD_KEY_NONE;
    int event_state = KEYPAD_RELEASE;

    if (!keypad_pop_event(&value, &event_state)) {
        return false;
    }

    if (c) {
        *c = value;
    }
    if (state) {
        *state = event_state;
    }

    keypad_curr_val = value;
    keypad_state = event_state;
    return true;
} 

void keypad_set_flag(void)
{
    keypad_update = false;
}

const char *keypad_key_name(char c)
{
    static char label[2] = {0};
    switch (c) {
        case KEYPAD_KEY_DEL: return "DEL";
        case KEYPAD_KEY_SPACE: return "SPACE";
        case KEYPAD_KEY_ALT: return "ALT";
        case KEYPAD_KEY_ENT: return "ENT";
        case KEYPAD_KEY_UP: return "UP";
        case KEYPAD_KEY_SYM: return "sym";
        case KEYPAD_KEY_NONE: return "NONE";
        default:
            label[0] = c;
            label[1] = '\0';
            return label;
    }
}

void keypad_loop(void)
{
    char c = -1;
    int state = -1;
    int row, col;
    int v = keypad.available();
    if (v <= 0) {
        return;
    }

    int k = keypad.getEvent();

    if(k >=KEYPAD_RELEASE_VAL_MIN && k <= KEYPAD_RELEASE_VAL_MAX){ // release event
        k = k - KEYPAD_RELEASE_VAL_MIN;
        state = KEYPAD_RELEASE;
    }   

    if(k >=KEYPAD_PRESS_VAL_MIN && k <= KEYPAD_PRESS_VAL_MAX){ // press event
        k = k - KEYPAD_PRESS_VAL_MIN;
        state = KEYPAD_PRESS;
    }

    if(state != -1){
        row = k / KEYPAD_COLS;
        col = (KEYPAD_COLS-1) - k % KEYPAD_COLS;
        c = keymap[row][col];
        if (c == KEYPAD_KEY_NONE) {
            return;
        }
        Serial.printf("k=%d, v=%d, press:%d, %d, %s\n", k, v, row, col, keypad_key_name(c));
        // if(keypad_listener)
        //     keypad_listener(state, c);
        
        keypad_curr_val = c;
        keypad_state = state;
        keypad_update = true;
        keypad_push_event(c, state);
    }
}

void keypad_regetser_cb(keypad_cb cb)
{
    keypad_listener = cb;
}
