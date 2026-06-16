#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

#include <Fonts/FreeMonoBold9pt7b.h>
#include <ExtensionIOXL9555.hpp>
#include <GxEPD2_BW.h>
#include <HynTouch.h>
#include <TouchDrvCSTXXX.hpp>

#include "utilities.h"

namespace {

GxEPD2_BW<GxEPD2_310_GDEQ031T10, GxEPD2_310_GDEQ031T10::HEIGHT> display(
    GxEPD2_310_GDEQ031T10(BOARD_EPD_CS, BOARD_EPD_DC, BOARD_EPD_RST, BOARD_EPD_BUSY));
ExtensionIOXL9555 xl9555_io;
TouchDrvCSTXXX legacy_touch;

constexpr uint8_t kTouchKeyCount = 3;
constexpr uint8_t kTargetCount = 5;
constexpr uint16_t kPatternHoldMs = 900;
constexpr uint16_t kPassHoldMs = 2000;
constexpr uint16_t kMinRenderIntervalMs = 140;
constexpr uint8_t kFullRefreshEvery = 10;
constexpr int16_t kTargetRadius = 16;
constexpr int16_t kTargetHitRadius = 24;
constexpr int16_t kCanvasTop = 50;
constexpr int16_t kCanvasBottom = 276;
constexpr int16_t kKeyTop = 284;
constexpr int16_t kTrailCellSize = 6;
constexpr int16_t kTrailCols = (LCD_HOR_SIZE + kTrailCellSize - 1) / kTrailCellSize;
constexpr int16_t kTrailRows = (LCD_VER_SIZE + kTrailCellSize - 1) / kTrailCellSize;

struct TouchTarget {
    int16_t x;
    int16_t y;
    const char *label;
    bool hit;
};

struct TestState {
    int16_t last_x = -1;
    int16_t last_y = -1;
    uint8_t last_points = 0;
    bool touch_seen = false;
    bool touch_ready = false;
    bool key_seen[kTouchKeyCount] = {false, false, false};
    bool key_down[kTouchKeyCount] = {false, false, false};
    bool pass_latched = false;
    bool render_pending = false;
    uint32_t pass_since = 0;
    uint32_t last_render_at = 0;
    uint8_t partial_refresh_count = 0;
};

enum TouchBackend : uint8_t {
    TOUCH_BACKEND_NONE = 0,
    TOUCH_BACKEND_HYN,
    TOUCH_BACKEND_LEGACY,
};

TouchTarget g_targets[kTargetCount];
bool g_trail[kTrailRows][kTrailCols] = {};
TestState g_state;
TouchBackend g_touch_backend = TOUCH_BACKEND_NONE;
bool g_xl9555_ready = false;
bool g_touch_i2c_ready = false;
char g_diag_line_1[40] = "XL9555: --";
char g_diag_line_2[40] = "TOUCH 0x1A: --";
char g_diag_line_3[40] = "DRV: --";

void prepareSharedSpiBus()
{
    pinMode(BOARD_LORA_CS, OUTPUT);
    digitalWrite(BOARD_LORA_CS, HIGH);

    pinMode(BOARD_LORA_RST, OUTPUT);
    digitalWrite(BOARD_LORA_RST, HIGH);

    pinMode(BOARD_SD_CS, OUTPUT);
    digitalWrite(BOARD_SD_CS, HIGH);

    pinMode(BOARD_EPD_CS, OUTPUT);
    digitalWrite(BOARD_EPD_CS, HIGH);
}

void setExpanderPin(uint8_t pin, bool high)
{
    xl9555_io.pinMode(pin, OUTPUT);
    xl9555_io.digitalWrite(pin, high ? HIGH : LOW);
}

bool initTouchPowerDomain()
{
    g_xl9555_ready = xl9555_io.init(Wire, BOARD_I2C_SDA, BOARD_I2C_SCL, XL9555_SLAVE_ADDRESS0);
    if (!g_xl9555_ready) {
        snprintf(g_diag_line_1, sizeof(g_diag_line_1), "XL9555: FAIL");
        return false;
    }

    snprintf(g_diag_line_1, sizeof(g_diag_line_1), "XL9555: OK");

    const uint8_t output_pins[] = {
        BOARD_XL9555_00_6609_EN,
        BOARD_XL9555_01_LORA_EN,
        BOARD_XL9555_02_GPS_EN,
        BOARD_XL9555_03_1V8_EN,
        BOARD_XL9555_04_LORA_SEL,
        BOARD_XL9555_05_MOTOR_EN,
        BOARD_XL9555_06_AMPLIFIER,
        BOARD_XL9555_07_TOUCH_RST,
        BOARD_XL9555_10_PWRKEY_EN,
        BOARD_XL9555_11_KEY_RST,
        BOARD_XL9555_12_AUDIO_SEL,
    };

    for (uint8_t pin : output_pins) {
        xl9555_io.pinMode(pin, OUTPUT);
        xl9555_io.digitalWrite(pin, HIGH);
        delay(1);
    }

    setExpanderPin(BOARD_XL9555_07_TOUCH_RST, false);
    delay(20);
    setExpanderPin(BOARD_XL9555_07_TOUCH_RST, true);
    delay(60);
    return true;
}

bool probeI2cAddress(uint8_t address)
{
    Wire.beginTransmission(address);
    return Wire.endTransmission() == 0;
}

bool initHynTouchDriver()
{
    if (!g_xl9555_ready) {
        return false;
    }

    setExpanderPin(BOARD_XL9555_07_TOUCH_RST, false);
    delay(20);
    setExpanderPin(BOARD_XL9555_07_TOUCH_RST, true);
    delay(60);

    hyn_touch_attach_xl9555(&xl9555_io);
    return hyn_touch_init();
}

bool initLegacyTouchDriver()
{
    if (g_xl9555_ready) {
        setExpanderPin(BOARD_XL9555_07_TOUCH_RST, false);
        delay(20);
        setExpanderPin(BOARD_XL9555_07_TOUCH_RST, true);
        delay(60);
    }

    legacy_touch.setPins(-1, BOARD_TOUCH_INT);
    if (!legacy_touch.begin(Wire, BOARD_I2C_ADDR_TOUCH, BOARD_TOUCH_SDA, BOARD_TOUCH_SCL)) {
        return false;
    }

    legacy_touch.disableAutoSleep();
    legacy_touch.setMaxCoordinates(LCD_HOR_SIZE, LCD_VER_SIZE);
    return true;
}

bool initBestTouchDriver()
{
    const bool hyn_ready = initHynTouchDriver();
    if (hyn_ready) {
        g_touch_backend = TOUCH_BACKEND_HYN;
        g_touch_i2c_ready = true;
        snprintf(g_diag_line_2, sizeof(g_diag_line_2), "TOUCH 0x1A: OK");
        snprintf(g_diag_line_3, sizeof(g_diag_line_3), "DRV: HYN OK");
        return true;
    }

    const bool legacy_ready = initLegacyTouchDriver();
    g_touch_i2c_ready = probeI2cAddress(BOARD_I2C_ADDR_TOUCH);
    snprintf(g_diag_line_2, sizeof(g_diag_line_2), "TOUCH 0x1A: %s", g_touch_i2c_ready ? "OK" : "FAIL");
    if (legacy_ready) {
        g_touch_backend = TOUCH_BACKEND_LEGACY;
        snprintf(g_diag_line_3, sizeof(g_diag_line_3), "DRV: CSTXXX OK");
        return true;
    }

    g_touch_backend = TOUCH_BACKEND_NONE;
    snprintf(g_diag_line_3, sizeof(g_diag_line_3), "DRV: HYN/CST FAIL");
    return false;
}

void initTargets()
{
    g_targets[0] = {28, 78, "1", false};
    g_targets[1] = {212, 78, "2", false};
    g_targets[2] = {120, 164, "3", false};
    g_targets[3] = {28, 248, "4", false};
    g_targets[4] = {212, 248, "5", false};
}

void clearTrail()
{
    memset(g_trail, 0, sizeof(g_trail));
}

void resetInteractiveState()
{
    g_state.last_x = -1;
    g_state.last_y = -1;
    g_state.last_points = 0;
    g_state.touch_seen = false;
    g_state.pass_latched = false;
    g_state.render_pending = false;
    g_state.pass_since = 0;
    g_state.partial_refresh_count = 0;
    memset(g_state.key_seen, 0, sizeof(g_state.key_seen));
    memset(g_state.key_down, 0, sizeof(g_state.key_down));
    if (g_touch_backend == TOUCH_BACKEND_HYN) {
        hyn_touch_clear_key_seen();
    }
    clearTrail();
    initTargets();
}

bool allTargetsHit()
{
    for (const TouchTarget &target : g_targets) {
        if (!target.hit) {
            return false;
        }
    }
    return true;
}

bool allKeysSeen()
{
    for (bool seen : g_state.key_seen) {
        if (!seen) {
            return false;
        }
    }
    return true;
}

bool allChecksPassed()
{
    if (!g_state.touch_ready || !allTargetsHit()) {
        return false;
    }
    return g_touch_backend != TOUCH_BACKEND_HYN || allKeysSeen();
}

int hitTargetCount()
{
    int count = 0;
    for (const TouchTarget &target : g_targets) {
        if (target.hit) {
            ++count;
        }
    }
    return count;
}

int keySeenCount()
{
    int count = 0;
    for (bool seen : g_state.key_seen) {
        if (seen) {
            ++count;
        }
    }
    return count;
}

bool markTargetIfHit(int16_t x, int16_t y)
{
    for (TouchTarget &target : g_targets) {
        if (target.hit) {
            continue;
        }
        const int32_t dx = x - target.x;
        const int32_t dy = y - target.y;
        if ((dx * dx) + (dy * dy) <= (kTargetHitRadius * kTargetHitRadius)) {
            target.hit = true;
            return true;
        }
    }
    return false;
}

void markTrailPoint(int16_t x, int16_t y)
{
    if (x < 0 || x >= LCD_HOR_SIZE || y < kCanvasTop || y >= kCanvasBottom) {
        return;
    }
    const int16_t col = x / kTrailCellSize;
    const int16_t row = y / kTrailCellSize;
    if (row >= 0 && row < kTrailRows && col >= 0 && col < kTrailCols) {
        g_trail[row][col] = true;
    }
}

void drawTitle()
{
    display.setTextColor(GxEPD_BLACK);
    display.setFont(&FreeMonoBold9pt7b);
    display.setCursor(8, 20);
    display.print("TOUCH TEST");
    display.setFont(nullptr);
    display.setCursor(8, 34);
    if (g_touch_backend == TOUCH_BACKEND_HYN) {
        display.print("Hit 5 circles + 3 touch keys");
    } else if (g_touch_backend == TOUCH_BACKEND_LEGACY) {
        display.print("Hit 5 circles on the screen");
    } else {
        display.print("Touch init failed");
    }
}

void drawStatusLine()
{
    display.drawLine(0, 40, LCD_HOR_SIZE - 1, 40, GxEPD_BLACK);
    display.setCursor(8, 47);
    display.print("TG ");
    display.print(hitTargetCount());
    display.print("/");
    display.print(kTargetCount);

    display.setCursor(80, 47);
    display.print("KY ");
    display.print(keySeenCount());
    display.print("/");
    display.print(kTouchKeyCount);

    display.setCursor(146, 47);
    if (g_state.touch_seen) {
        display.print(g_state.last_x);
        display.print(",");
        display.print(g_state.last_y);
    } else {
        display.print("--,--");
    }

    display.setCursor(194, 47);
    if (g_touch_backend == TOUCH_BACKEND_HYN) {
        display.print("H");
    } else if (g_touch_backend == TOUCH_BACKEND_LEGACY) {
        display.print("C");
    } else {
        display.print("-");
    }
}

void drawTouchCanvas()
{
    display.drawRect(4, kCanvasTop, LCD_HOR_SIZE - 8, kCanvasBottom - kCanvasTop, GxEPD_BLACK);

    for (int16_t row = 0; row < kTrailRows; ++row) {
        for (int16_t col = 0; col < kTrailCols; ++col) {
            if (!g_trail[row][col]) {
                continue;
            }
            const int16_t x = (col * kTrailCellSize) + 1;
            const int16_t y = (row * kTrailCellSize) + 1;
            display.fillRect(x, y, kTrailCellSize - 2, kTrailCellSize - 2, GxEPD_BLACK);
        }
    }

    display.setFont(nullptr);
    for (const TouchTarget &target : g_targets) {
        if (target.hit) {
            display.fillCircle(target.x, target.y, kTargetRadius, GxEPD_BLACK);
            display.setTextColor(GxEPD_WHITE);
        } else {
            display.drawCircle(target.x, target.y, kTargetRadius, GxEPD_BLACK);
            display.drawFastHLine(target.x - 6, target.y, 12, GxEPD_BLACK);
            display.drawFastVLine(target.x, target.y - 6, 12, GxEPD_BLACK);
            display.setTextColor(GxEPD_BLACK);
        }

        display.setCursor(target.x - 3, target.y + 4);
        display.print(target.label);
        display.setTextColor(GxEPD_BLACK);
    }
}

void drawKeyBar()
{
    if (g_touch_backend != TOUCH_BACKEND_HYN) {
        display.drawLine(0, kKeyTop - 6, LCD_HOR_SIZE - 1, kKeyTop - 6, GxEPD_BLACK);
        display.setCursor(10, kKeyTop + 18);
        display.print("Legacy touch mode: screen touch only");
        return;
    }

    display.drawLine(0, kKeyTop - 6, LCD_HOR_SIZE - 1, kKeyTop - 6, GxEPD_BLACK);
    for (uint8_t i = 0; i < kTouchKeyCount; ++i) {
        const int16_t box_x = 8 + (i * 76);
        const int16_t box_y = kKeyTop;
        const int16_t box_w = 68;
        const int16_t box_h = 28;

        if (g_state.key_down[i]) {
            display.fillRect(box_x, box_y, box_w, box_h, GxEPD_BLACK);
            display.setTextColor(GxEPD_WHITE);
        } else {
            display.drawRect(box_x, box_y, box_w, box_h, GxEPD_BLACK);
            display.setTextColor(GxEPD_BLACK);
            if (g_state.key_seen[i]) {
                display.fillRect(box_x + 4, box_y + 4, 8, 8, GxEPD_BLACK);
            }
        }

        display.setCursor(box_x + 18, box_y + 18);
        display.print("K");
        display.print(i + 1);
        if (g_state.key_down[i]) {
            display.print(" DN");
        } else if (g_state.key_seen[i]) {
            display.print(" OK");
        } else {
            display.print(" --");
        }
        display.setTextColor(GxEPD_BLACK);
    }
}

void drawPassBanner()
{
    if (!g_state.pass_latched) {
        return;
    }

    display.fillRect(42, 124, 156, 62, GxEPD_BLACK);
    display.drawRect(42, 124, 156, 62, GxEPD_BLACK);
    display.setTextColor(GxEPD_WHITE);
    display.setFont(&FreeMonoBold9pt7b);
    display.setCursor(78, 152);
    display.print("PASS");
    display.setFont(nullptr);
    display.setCursor(60, 172);
    display.print("display + touch ok");
    display.setTextColor(GxEPD_BLACK);
}

void drawInteractiveScreen()
{
    display.fillScreen(GxEPD_WHITE);
    drawTitle();
    drawStatusLine();
    drawTouchCanvas();
    drawKeyBar();
    drawPassBanner();
}

void renderInteractive(bool full_refresh)
{
    if (full_refresh) {
        display.setFullWindow();
    } else {
        display.setPartialWindow(0, 0, LCD_HOR_SIZE, LCD_VER_SIZE);
    }

    display.firstPage();
    do {
        drawInteractiveScreen();
    } while (display.nextPage());

    g_state.last_render_at = millis();
    if (full_refresh) {
        g_state.partial_refresh_count = 0;
    } else {
        ++g_state.partial_refresh_count;
    }
}

void drawPatternBlack()
{
    display.fillScreen(GxEPD_BLACK);
}

void drawPatternWhite()
{
    display.fillScreen(GxEPD_WHITE);
}

void drawPatternVerticalBars()
{
    display.fillScreen(GxEPD_WHITE);
    for (int16_t x = 0; x < LCD_HOR_SIZE; x += 24) {
        display.fillRect(x, 0, 12, LCD_VER_SIZE, GxEPD_BLACK);
    }
}

void drawPatternHorizontalBars()
{
    display.fillScreen(GxEPD_WHITE);
    for (int16_t y = 0; y < LCD_VER_SIZE; y += 24) {
        display.fillRect(0, y, LCD_HOR_SIZE, 12, GxEPD_BLACK);
    }
}

void drawPatternCheckerboard()
{
    display.fillScreen(GxEPD_WHITE);
    for (int16_t y = 0; y < LCD_VER_SIZE; y += 24) {
        for (int16_t x = 0; x < LCD_HOR_SIZE; x += 24) {
            if ((((x / 24) + (y / 24)) & 1) == 0) {
                display.fillRect(x, y, 24, 24, GxEPD_BLACK);
            }
        }
    }
}

void showPattern(void (*pattern_drawer)())
{
    display.setFullWindow();
    display.firstPage();
    do {
        pattern_drawer();
    } while (display.nextPage());
    delay(kPatternHoldMs);
}

void runDisplayBootPatterns()
{
    showPattern(drawPatternBlack);
    showPattern(drawPatternWhite);
    showPattern(drawPatternVerticalBars);
    showPattern(drawPatternHorizontalBars);
    showPattern(drawPatternCheckerboard);
}

bool pollTouchState()
{
    bool changed = false;

    int16_t x = 0;
    int16_t y = 0;
    uint8_t touched = 0;
    if (g_state.touch_ready) {
        if (g_touch_backend == TOUCH_BACKEND_HYN) {
            touched = hyn_touch_get_point(&x, &y, 1);
        } else if (g_touch_backend == TOUCH_BACKEND_LEGACY) {
            touched = legacy_touch.getPoint(&x, &y, 1);
        }
    }
    if (touched > 0) {
        if (!g_state.touch_seen || g_state.last_x != x || g_state.last_y != y || g_state.last_points != touched) {
            changed = true;
        }
        g_state.touch_seen = true;
        g_state.last_x = x;
        g_state.last_y = y;
        g_state.last_points = touched;
        markTrailPoint(x, y);
        if (markTargetIfHit(x, y)) {
            changed = true;
        }
    }

    if (g_touch_backend == TOUCH_BACKEND_HYN) {
        for (uint8_t i = 0; i < kTouchKeyCount; ++i) {
            const bool down = hyn_touch_get_key_state(i);
            const bool seen = down || hyn_touch_get_key_seen(i);
            if (g_state.key_down[i] != down) {
                g_state.key_down[i] = down;
                changed = true;
            }
            if (seen && !g_state.key_seen[i]) {
                g_state.key_seen[i] = true;
                changed = true;
            }
        }
    }

    if (!g_state.pass_latched && allChecksPassed()) {
        g_state.pass_latched = true;
        g_state.pass_since = millis();
        changed = true;
    }

    return changed;
}

bool shouldRenderNow()
{
    if (!g_state.render_pending) {
        return false;
    }
    return millis() - g_state.last_render_at >= kMinRenderIntervalMs;
}

void updateTouchTest()
{
    if (pollTouchState()) {
        g_state.render_pending = true;
    }

    if (g_state.pass_latched && millis() - g_state.pass_since >= kPassHoldMs) {
        resetInteractiveState();
        renderInteractive(true);
        return;
    }

    if (!shouldRenderNow()) {
        return;
    }

    const bool full_refresh = g_state.partial_refresh_count >= kFullRefreshEvery;
    renderInteractive(full_refresh);
    g_state.render_pending = false;
}

void initDisplay()
{
    prepareSharedSpiBus();
    analogWrite(BOARD_EPD_BL, 0);
    analogWrite(BOARD_KEYBOARD_LED, 0);

    SPI.begin(BOARD_SPI_SCK, BOARD_SPI_MISO, BOARD_SPI_MOSI);
    display.init(115200, true, 2, false);
    display.setRotation(0);
    display.setTextColor(GxEPD_BLACK);
    display.setFont(nullptr);
}

void showTouchInitFailure()
{
    display.setFullWindow();
    display.firstPage();
    do {
        display.fillScreen(GxEPD_WHITE);
        display.setTextColor(GxEPD_BLACK);
        display.setFont(&FreeMonoBold9pt7b);
        display.setCursor(8, 24);
        display.print("TOUCH FAIL");
        display.setFont(nullptr);
        display.setCursor(8, 48);
        display.print(g_diag_line_1);
        display.setCursor(8, 64);
        display.print(g_diag_line_2);
        display.setCursor(8, 80);
        display.print(g_diag_line_3);
        display.setCursor(8, 102);
        display.print("Display path is still alive");
    } while (display.nextPage());
}

} // namespace

void setup()
{
    setCpuFrequencyMhz(240);
    Serial.begin(115200);
    delay(80);

    Wire.begin(BOARD_I2C_SDA, BOARD_I2C_SCL);
    initDisplay();
    runDisplayBootPatterns();

    initTouchPowerDomain();
    g_state.touch_ready = initBestTouchDriver();

    if (!g_state.touch_ready) {
        showTouchInitFailure();
        return;
    }

    resetInteractiveState();
    g_state.touch_ready = true;
    renderInteractive(true);
}

void loop()
{
    if (!g_state.touch_ready) {
        delay(100);
        return;
    }

    updateTouchTest();
    delay(5);
}
