#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_TCA8418.h>
#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <TDeckMaxBoard.h>

#define KEYPAD_ROWS 4
#define KEYPAD_COLS 10

#define KEY_NONE '\0'
#define KEY_DEL  '\b'
#define KEY_ALT  '\x01'
#define KEY_ENT  '\n'
#define KEY_UP   '\x02'
#define KEY_SYM  '\x03'
#define KEY_MIC  '\x04'
#define KEY_SPK  '\x05'

#define LONGPRESS_MS       600
#define INPUT_MAX_LEN      96
#define RENDER_INTERVAL_MS 180
#define FULL_REFRESH_EVERY 30

// Dynamic-region geometry (the only area that re-renders on input changes).
#define DYN_X 0
#define DYN_Y 44
#define DYN_W LCD_HOR_SIZE
#define DYN_H 102        // covers status bar + input box, ends at y=146

#define MODE_BAR_Y 50
#define INPUT_BOX_X 8
#define INPUT_BOX_Y 73
#define INPUT_BOX_W (LCD_HOR_SIZE - 16)
#define INPUT_BOX_H 76

GxEPD2_BW<GxEPD2_310_GDEQ031T10, GxEPD2_310_GDEQ031T10::HEIGHT> display(
    GxEPD2_310_GDEQ031T10(BOARD_EPD_CS, BOARD_EPD_DC, BOARD_EPD_RST, BOARD_EPD_BUSY));

Adafruit_TCA8418 keypad;

static const char keymap_lower[KEYPAD_ROWS][KEYPAD_COLS] = {
    {'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p'},
    {'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', KEY_DEL},
    {KEY_ALT, 'z', 'x', 'c', 'v', 'b', 'n', 'm', '$', KEY_ENT},
    {KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE, KEY_UP, KEY_MIC, ' ', KEY_SYM, KEY_UP},
};

static const char keymap_upper[KEYPAD_ROWS][KEYPAD_COLS] = {
    {'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P'},
    {'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', KEY_DEL},
    {KEY_ALT, 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '$', KEY_ENT},
    {KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE, KEY_UP, KEY_MIC, ' ', KEY_SYM, KEY_UP},
};

static const char keymap_sym[KEYPAD_ROWS][KEYPAD_COLS] = {
    {'#', '1', '2', '3', '(', ')', '_', '-', '+', '@'},
    {'*', '4', '5', '6', '/', ':', ';', '\'', '"', KEY_DEL},
    {KEY_ALT, '7', '8', '9', '?', '!', ',', '.', KEY_SPK, KEY_ENT},
    {KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE, KEY_UP, KEY_MIC, ' ', KEY_SYM, KEY_UP},
};

enum KeyMode {
    MODE_LOWER = 0,
    MODE_UPPER,
};

struct KeyState {
    KeyMode mode = MODE_LOWER;
    bool up_held = false;
    bool password_hidden = false;

    char input[INPUT_MAX_LEN + 1] = {0};
    int  input_len = 0;
    char status[32] = "Ready";

    char  held_key = KEY_NONE;
    uint32_t held_since = 0;
    bool  long_fired = false;

    // Snapshot of last rendered values: only re-render when these change.
    KeyMode last_mode = MODE_LOWER;
    bool last_up_held = false;
    bool last_password_hidden = false;
    int  last_input_len = -1;
    uint32_t last_input_hash = 0;

    bool dirty = true;
    bool shell_drawn = false;
    uint32_t last_render_at = 0;
    uint8_t partial_count = 0;
};

static KeyState g_ks;

static char resolveKey(int row, int col)
{
    if (row < 0 || row >= KEYPAD_ROWS || col < 0 || col >= KEYPAD_COLS) {
        return KEY_NONE;
    }
    if (g_ks.up_held) {
        return keymap_sym[row][col];
    }
    if (g_ks.mode == MODE_UPPER) {
        return keymap_upper[row][col];
    }
    return keymap_lower[row][col];
}

static const char *modeLabel()
{
    if (g_ks.up_held)              return "SYM";
    return g_ks.mode == MODE_UPPER ? "ABC" : "abc";
}

static uint32_t inputHash()
{
    uint32_t h = 2166136261u;
    for (int i = 0; i < g_ks.input_len; ++i) {
        h ^= (uint8_t)g_ks.input[i];
        h *= 16777619u;
    }
    return h;
}

static void setStatus(const char *text)
{
    if (text == nullptr) return;
    snprintf(g_ks.status, sizeof(g_ks.status), "%s", text);
    g_ks.dirty = true;
}

static void inputAppend(char c)
{
    if (g_ks.input_len >= INPUT_MAX_LEN) return;
    g_ks.input[g_ks.input_len++] = c;
    g_ks.input[g_ks.input_len] = '\0';
}

static void inputBackspace()
{
    if (g_ks.input_len <= 0) return;
    g_ks.input[--g_ks.input_len] = '\0';
}

static void inputClear()
{
    g_ks.input_len = 0;
    g_ks.input[0] = '\0';
}

static void prepareSharedSpiBus()
{
    pinMode(BOARD_LORA_CS, OUTPUT);  digitalWrite(BOARD_LORA_CS, HIGH);
    pinMode(BOARD_LORA_RST, OUTPUT); digitalWrite(BOARD_LORA_RST, HIGH);
    pinMode(BOARD_SD_CS, OUTPUT);    digitalWrite(BOARD_SD_CS, HIGH);
    pinMode(BOARD_EPD_CS, OUTPUT);   digitalWrite(BOARD_EPD_CS, HIGH);
}

static void drawTextAt(int16_t x, int16_t y, const char *text, const GFXfont *font = nullptr)
{
    display.setFont(font);
    display.setTextColor(GxEPD_BLACK);
    display.setCursor(x, y);
    display.print(text);
}

// Static UI: title, separators, hints, footer label.
// Drawn once per full refresh; never touched by partial updates.
static void drawShell()
{
    display.fillScreen(GxEPD_WHITE);

    // Header
    drawTextAt(8, 22, "Keypad Demo", &FreeMonoBold9pt7b);
    display.drawLine(0, 30, LCD_HOR_SIZE - 1, 30, GxEPD_BLACK);

    // Section header above the input area
    drawTextAt(8, 42, "Input");

    // Section divider above hint list
    display.drawLine(0, 152, LCD_HOR_SIZE - 1, 152, GxEPD_BLACK);
    drawTextAt(8, 164, "Shortcuts", &FreeMonoBold9pt7b);

    // Hints (single column, no overlap)
    int16_t y = 184;
    drawTextAt(8, y, "ALT      switch abc / ABC");                y += 14;
    drawTextAt(8, y, "Hold UP  type symbol layer");               y += 14;
    drawTextAt(8, y, "DEL      backspace, hold to clear");        y += 14;
    drawTextAt(8, y, "ENT      confirm input (UART)");            y += 14;
    drawTextAt(8, y, "SYM      tap = mask, hold = cancel");       y += 14;

    // Footer divider
    display.drawLine(0, LCD_VER_SIZE - 16, LCD_HOR_SIZE - 1, LCD_VER_SIZE - 16, GxEPD_BLACK);
}

static void drawStatusBar()
{
    char buf[40];

    drawTextAt(8, MODE_BAR_Y, "Mode:");
    drawTextAt(44, MODE_BAR_Y, modeLabel(), &FreeMonoBold9pt7b);

    snprintf(buf, sizeof(buf), "Mask: %s", g_ks.password_hidden ? "ON" : "OFF");
    drawTextAt(96, MODE_BAR_Y, buf);

    snprintf(buf, sizeof(buf), "%d/%d", g_ks.input_len, INPUT_MAX_LEN);
    drawTextAt(LCD_HOR_SIZE - 50, MODE_BAR_Y, buf);

    drawTextAt(8, MODE_BAR_Y + 12, g_ks.status);
}

static void drawInputBoxContent()
{
    // Frame
    display.drawRect(INPUT_BOX_X, INPUT_BOX_Y, INPUT_BOX_W, INPUT_BOX_H, GxEPD_BLACK);

    const int16_t inner_x = INPUT_BOX_X + 6;
    const int16_t inner_y = INPUT_BOX_Y + 6;
    const int16_t inner_w = INPUT_BOX_W - 12;
    const int16_t inner_h = INPUT_BOX_H - 12;
    const int16_t char_w  = 6;
    const int16_t line_h  = 11;
    const int16_t cols_per_line = inner_w / char_w;
    const int16_t max_lines = inner_h / line_h;
    const int16_t max_chars = cols_per_line * max_lines;

    display.setFont(nullptr);
    display.setTextColor(GxEPD_BLACK);

    if (g_ks.input_len == 0) {
        display.setCursor(inner_x, inner_y + 2);
        display.print("(input empty)");
        return;
    }

    int start = 0;
    if (g_ks.input_len > max_chars) start = g_ks.input_len - max_chars;
    int total = g_ks.input_len - start;

    for (int line = 0; line < max_lines; ++line) {
        int offset = line * cols_per_line;
        if (offset >= total) break;
        int count = total - offset;
        if (count > cols_per_line) count = cols_per_line;
        display.setCursor(inner_x, inner_y + 2 + line * line_h);
        for (int i = 0; i < count; ++i) {
            char ch = g_ks.input[start + offset + i];
            display.print(g_ks.password_hidden ? '*' : ch);
        }
    }
}

static void drawDynamicArea()
{
    // Erase only the dynamic window so partial flush has minimal area.
    display.fillRect(DYN_X, DYN_Y, DYN_W, DYN_H, GxEPD_WHITE);
    drawStatusBar();
    drawInputBoxContent();
}

static void renderFull()
{
    display.setFullWindow();
    display.firstPage();
    do {
        drawShell();
        drawDynamicArea();
    } while (display.nextPage());
    g_ks.last_render_at = millis();
    g_ks.partial_count = 0;
    g_ks.shell_drawn = true;
}

static void renderDynamicPartial()
{
    display.setPartialWindow(DYN_X, DYN_Y, DYN_W, DYN_H);
    display.firstPage();
    do {
        drawDynamicArea();
    } while (display.nextPage());
    g_ks.last_render_at = millis();
    ++g_ks.partial_count;
}

static bool dynamicChanged()
{
    if (g_ks.mode != g_ks.last_mode) return true;
    if (g_ks.up_held != g_ks.last_up_held) return true;
    if (g_ks.password_hidden != g_ks.last_password_hidden) return true;
    if (g_ks.input_len != g_ks.last_input_len) return true;
    if (inputHash() != g_ks.last_input_hash) return true;
    return false;
}

static void snapshotState()
{
    g_ks.last_mode = g_ks.mode;
    g_ks.last_up_held = g_ks.up_held;
    g_ks.last_password_hidden = g_ks.password_hidden;
    g_ks.last_input_len = g_ks.input_len;
    g_ks.last_input_hash = inputHash();
}

static void renderIfNeeded()
{
    if (!g_ks.dirty && !dynamicChanged()) return;
    if (millis() - g_ks.last_render_at < RENDER_INTERVAL_MS) return;

    if (!g_ks.shell_drawn || g_ks.partial_count >= FULL_REFRESH_EVERY) {
        renderFull();
    } else {
        renderDynamicPartial();
    }
    snapshotState();
    g_ks.dirty = false;
}

static void handleShortPress(char c)
{
    if (c == KEY_NONE) return;
    if (c == KEY_UP)   return;
    if (c == KEY_MIC || c == KEY_SPK) return;

    if (c == KEY_ALT) {
        g_ks.mode = (g_ks.mode == MODE_LOWER) ? MODE_UPPER : MODE_LOWER;
        setStatus("ALT");
        return;
    }
    if (c == KEY_DEL) {
        inputBackspace();
        setStatus("DEL");
        return;
    }
    if (c == KEY_ENT) {
        Serial.print("[ENTER] ");
        Serial.println(g_ks.input);
        setStatus(g_ks.input_len > 0 ? "ENT confirmed" : "ENT empty");
        return;
    }
    if (c == KEY_SYM) {
        g_ks.password_hidden = !g_ks.password_hidden;
        setStatus(g_ks.password_hidden ? "SYM mask on" : "SYM mask off");
        return;
    }

    inputAppend(c);
    char status[12] = "Key: ";
    status[5] = c;
    status[6] = '\0';
    setStatus(status);
}

static void handleLongPress(char c)
{
    if (c == KEY_DEL) {
        inputClear();
        setStatus("DEL clear");
        return;
    }
    if (c == KEY_SYM) {
        inputClear();
        g_ks.password_hidden = false;
        setStatus("SYM cancel");
        Serial.println("[CANCEL]");
    }
}

static void processKeypadEvents()
{
    while (keypad.available() > 0) {
        int k = keypad.getEvent();
        bool pressed = (k & 0x80) != 0;
        k &= 0x7F;
        k--;
        if (k < 0) continue;
        int row = k / KEYPAD_COLS;
        int col = (KEYPAD_COLS - 1) - (k % KEYPAD_COLS);
        char c = resolveKey(row, col);
        if (c == KEY_NONE) continue;

        if (pressed) {
            if (c == KEY_UP) {
                g_ks.up_held = true;
                continue;
            }

            if (c == KEY_DEL || c == KEY_SYM) {
                g_ks.held_key = c;
                g_ks.held_since = millis();
                g_ks.long_fired = false;
            } else {
                g_ks.held_key = KEY_NONE;
                g_ks.long_fired = false;
                handleShortPress(c);
            }
        } else {
            if (c == KEY_UP) {
                g_ks.up_held = false;
                continue;
            }
            if (g_ks.held_key == c && !g_ks.long_fired) {
                handleShortPress(g_ks.held_key);
            }
            if (g_ks.held_key == c) {
                g_ks.held_key = KEY_NONE;
                g_ks.long_fired = false;
            }
        }
    }
}

static void checkLongPress()
{
    if (g_ks.held_key == KEY_NONE || g_ks.long_fired) return;
    if (millis() - g_ks.held_since >= LONGPRESS_MS) {
        handleLongPress(g_ks.held_key);
        g_ks.long_fired = true;
    }
}

void setup()
{
    Serial.begin(115200);
    delay(50);

    prepareSharedSpiBus();

    SPI.begin(BOARD_EPD_SCK, BOARD_SPI_MISO, BOARD_EPD_MOSI);
    display.init(115200, true, 2, false);
    display.setRotation(0);
    display.setTextColor(GxEPD_BLACK);

    Wire.begin(BOARD_KEYBOARD_SDA, BOARD_KEYBOARD_SCL);
    if (!keypad.begin(BOARD_I2C_ADDR_KEYBOARD, &Wire)) {
        Serial.println("keypad not found, check wiring & pullups!");
        display.setFullWindow();
        display.firstPage();
        do {
            display.fillScreen(GxEPD_WHITE);
            drawTextAt(20, 60,  "Keypad init failed", &FreeMonoBold9pt7b);
            drawTextAt(20, 86,  "Check wiring & I2C pull-ups");
        } while (display.nextPage());
        while (true) { delay(1000); }
    }
    keypad.matrix(KEYPAD_ROWS, KEYPAD_COLS);
    keypad.flush();

    g_ks.dirty = true;
    renderIfNeeded();
}

void loop()
{
    processKeypadEvents();
    checkLongPress();
    renderIfNeeded();
    delay(5);
}
