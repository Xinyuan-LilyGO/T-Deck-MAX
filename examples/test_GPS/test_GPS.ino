#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <TinyGPS++.h>
#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <TDeckMaxBoard.h>
#include "ExtensionIOXL9555.hpp"

#define RENDER_INTERVAL_MS  5000
#define GPS_TIMEOUT_MS      30000

// Static header height (drawn once per full refresh).
#define SHELL_TOP_BAR_Y     34

// Single dynamic region — covers everything below the title/status bar.
// All values are repainted inside this window on every partial update.
#define DYN_X 0
#define DYN_Y SHELL_TOP_BAR_Y
#define DYN_W LCD_HOR_SIZE
#define DYN_H (LCD_VER_SIZE - SHELL_TOP_BAR_Y)

GxEPD2_BW<GxEPD2_310_GDEQ031T10, GxEPD2_310_GDEQ031T10::HEIGHT> display(
    GxEPD2_310_GDEQ031T10(BOARD_EPD_CS, BOARD_EPD_DC, BOARD_EPD_RST, BOARD_EPD_BUSY));

TinyGPSPlus gps;
ExtensionIOXL9555 io;
static uint8_t buffer[256];

struct GpsState {
    bool xl9555_ok = false;
    bool gps_link_ok = false;
    bool fix_valid = false;
    bool data_seen = false;

    uint32_t chars_total = 0;
    uint32_t sentences_total = 0;
    uint32_t failed_checksum = 0;
    uint32_t last_fix_at = 0;
    uint32_t last_data_at = 0;

    double lat = 0.0;
    double lng = 0.0;
    double altitude_m = 0.0;
    double speed_kmph = 0.0;
    double course_deg = 0.0;
    double hdop = 99.99;
    uint32_t satellites = 0;

    uint16_t year = 0;
    uint8_t  month = 0, day = 0;
    uint8_t  hour = 0, minute = 0, second = 0;
    bool date_valid = false;
    bool time_valid = false;

    // Last rendered status text (for header dirty check).
    const char *last_status = "";

    bool shell_drawn = false;
    uint32_t last_render_at = 0;
    uint8_t  partial_count = 0;
    uint32_t last_dyn_hash = 0;
};

static GpsState g;

static void prepareSharedSpiBus()
{
    pinMode(BOARD_LORA_CS, OUTPUT);  digitalWrite(BOARD_LORA_CS, HIGH);
    pinMode(BOARD_LORA_RST, OUTPUT); digitalWrite(BOARD_LORA_RST, HIGH);
    pinMode(BOARD_SD_CS, OUTPUT);    digitalWrite(BOARD_SD_CS, HIGH);
    pinMode(BOARD_EPD_CS, OUTPUT);   digitalWrite(BOARD_EPD_CS, HIGH);
}

static int getAck(uint8_t *buf, uint16_t size, uint8_t reqClass, uint8_t reqID);
static bool gps_recovery();
static bool gps_init_chip();

static const char *fixStatusText()
{
    if (!g.gps_link_ok) return "NO LINK";
    if (!g.data_seen)   return "WAITING";
    if (g.fix_valid)    return "FIX OK";
    return "ACQUIRING";
}

static void drawTextAt(int16_t x, int16_t y, const char *text, const GFXfont *font = nullptr)
{
    display.setFont(font);
    display.setTextColor(GxEPD_BLACK);
    display.setCursor(x, y);
    display.print(text);
}

// Static header: title + status — drawn once per full refresh.
static void drawShell()
{
    display.fillScreen(GxEPD_WHITE);
    drawTextAt(8, 22, "GPS", &FreeMonoBold9pt7b);

    // Status — same baseline as title, right-aligned so longer text
    // (e.g. "ACQUIRING") cannot overlap the title.
    const char *status = fixStatusText();
    int16_t  tbx, tby;
    uint16_t tbw, tbh;
    display.setFont(&FreeMonoBold9pt7b);
    display.getTextBounds(status, 0, 0, &tbx, &tby, &tbw, &tbh);
    int16_t sx = LCD_HOR_SIZE - 8 - (int16_t)tbw - tbx;
    if (sx < 132) sx = 132;
    drawTextAt(sx, 22, status, &FreeMonoBold9pt7b);

    display.drawLine(0, 30, LCD_HOR_SIZE - 1, 30, GxEPD_BLACK);
}

static void drawSectionHeader(int16_t y, const char *text)
{
    drawTextAt(8, y, text, &FreeMonoBold9pt7b);
}

static void drawRow(int16_t y, const char *label, const char *value)
{
    display.setFont(nullptr);
    display.setTextColor(GxEPD_BLACK);
    display.setCursor(10, y);
    display.print(label);
    display.setCursor(96, y);
    display.print(value);
}

// Build the body inside the dynamic window. Every text row is repainted
// fresh on each partial update — partial repaint also "re-energizes" the
// pixels, which prevents the visible fading between full refreshes.
static void drawDynamicBody()
{
    // White background only inside the dynamic window
    display.fillRect(DYN_X, DYN_Y, DYN_W, DYN_H, GxEPD_WHITE);

    char buf[40];
    int16_t y = DYN_Y + 14;

    // -- Section: Position --
    drawSectionHeader(y, g.fix_valid ? "Position [FIX]" : "Position [--]");
    y += 14;

    if (g.fix_valid) {
        snprintf(buf, sizeof(buf), "%.6f", g.lat);
        drawRow(y, "Lat:", buf); y += 12;
        snprintf(buf, sizeof(buf), "%.6f", g.lng);
        drawRow(y, "Lng:", buf); y += 12;
        snprintf(buf, sizeof(buf), "%.1f m", g.altitude_m);
        drawRow(y, "Alt:", buf); y += 12;
        snprintf(buf, sizeof(buf), "%.2f km/h", g.speed_kmph);
        drawRow(y, "Speed:", buf); y += 12;
        snprintf(buf, sizeof(buf), "%.1f deg", g.course_deg);
        drawRow(y, "Course:", buf); y += 12;
    } else {
        drawRow(y, "Lat:",    "--"); y += 12;
        drawRow(y, "Lng:",    "--"); y += 12;
        drawRow(y, "Alt:",    "--"); y += 12;
        drawRow(y, "Speed:",  "--"); y += 12;
        drawRow(y, "Course:", "--"); y += 12;
    }

    snprintf(buf, sizeof(buf), "%lu", (unsigned long)g.satellites);
    drawRow(y, "Sats:", buf); y += 12;
    if (g.hdop < 99.0) snprintf(buf, sizeof(buf), "%.2f", g.hdop);
    else               snprintf(buf, sizeof(buf), "--");
    drawRow(y, "HDOP:", buf); y += 14;

    // -- Section: Date / Time --
    display.drawLine(0, y - 6, LCD_HOR_SIZE - 1, y - 6, GxEPD_BLACK);
    drawSectionHeader(y + 6, "Date / Time (UTC)");
    y += 20;

    if (g.date_valid) snprintf(buf, sizeof(buf), "%04u-%02u-%02u", g.year, g.month, g.day);
    else              snprintf(buf, sizeof(buf), "----/--/--");
    drawRow(y, "Date:", buf); y += 12;

    if (g.time_valid) snprintf(buf, sizeof(buf), "%02u:%02u:%02u", g.hour, g.minute, g.second);
    else              snprintf(buf, sizeof(buf), "--:--:--");
    drawRow(y, "Time:", buf); y += 14;

    // -- Section: Diagnostics --
    display.drawLine(0, y - 6, LCD_HOR_SIZE - 1, y - 6, GxEPD_BLACK);
    drawSectionHeader(y + 6, "Diagnostics");
    y += 20;

    drawRow(y, "XL9555:", g.xl9555_ok   ? "OK" : "FAIL"); y += 12;
    drawRow(y, "Module:", g.gps_link_ok ? "OK" : "FAIL"); y += 12;

    snprintf(buf, sizeof(buf), "%lu chars", (unsigned long)g.chars_total);
    drawRow(y, "RX:", buf); y += 12;

    snprintf(buf, sizeof(buf), "%lu / %lu err",
             (unsigned long)g.sentences_total, (unsigned long)g.failed_checksum);
    drawRow(y, "NMEA:", buf); y += 12;

    if (g.last_data_at == 0) {
        snprintf(buf, sizeof(buf), "no data yet");
    } else {
        uint32_t since = (millis() - g.last_data_at) / 1000;
        snprintf(buf, sizeof(buf), "%lus ago", (unsigned long)since);
    }
    drawRow(y, "Last RX:", buf); y += 12;

    if (g.last_fix_at == 0) snprintf(buf, sizeof(buf), "never");
    else {
        uint32_t since = (millis() - g.last_fix_at) / 1000;
        snprintf(buf, sizeof(buf), "%lus ago", (unsigned long)since);
    }
    drawRow(y, "Last fix:", buf);
}

static void renderFull()
{
    display.setFullWindow();
    display.firstPage();
    do {
        drawShell();
        drawDynamicBody();
    } while (display.nextPage());
    // Sync previous buffer = current buffer so the next partial diff is clean.
    display.epd2.writeScreenBufferAgain();

    g.last_render_at = millis();
    g.partial_count = 0;
    g.shell_drawn = true;
    g.last_status = fixStatusText();
}

static void renderPartial()
{
    // If status text changed, the title bar must also redraw → full refresh.
    if (fixStatusText() != g.last_status) {
        renderFull();
        return;
    }

    // Pre-fill the previous buffer (RAM 0x10) with white BEFORE writing new
    // content. The UC8253 differential waveform only drives pixels where
    // prev≠curr. Forcing prev=0xFF (white) means every black pixel in the
    // current frame is a white→black transition and gets a full drive pulse,
    // preventing the grey-fade that accumulates when content is unchanged
    // across repeated partial refreshes.
    display.epd2.writeScreenBufferAgain(0xFF);

    display.setPartialWindow(DYN_X, DYN_Y, DYN_W, DYN_H);
    display.firstPage();
    do {
        drawDynamicBody();
    } while (display.nextPage());

    g.last_render_at = millis();
    ++g.partial_count;
}

static void renderTick()
{
    renderFull();
}

static void refreshGpsState()
{
    g.chars_total = gps.charsProcessed();
    g.sentences_total = gps.sentencesWithFix();
    g.failed_checksum = gps.failedChecksum();

    if (gps.location.isValid() && gps.location.isUpdated()) {
        g.lat = gps.location.lat();
        g.lng = gps.location.lng();
        g.fix_valid = true;
        g.last_fix_at = millis();
    } else if (gps.location.age() > 5000) {
        g.fix_valid = false;
    }

    if (gps.altitude.isValid())   g.altitude_m = gps.altitude.meters();
    if (gps.speed.isValid())      g.speed_kmph = gps.speed.kmph();
    if (gps.course.isValid())     g.course_deg = gps.course.deg();
    if (gps.hdop.isValid())       g.hdop = gps.hdop.hdop();
    if (gps.satellites.isValid()) g.satellites = gps.satellites.value();

    g.date_valid = gps.date.isValid();
    if (g.date_valid) {
        g.year  = gps.date.year();
        g.month = gps.date.month();
        g.day   = gps.date.day();
    }
    g.time_valid = gps.time.isValid();
    if (g.time_valid) {
        g.hour   = gps.time.hour();
        g.minute = gps.time.minute();
        g.second = gps.time.second();
    }
}

static void initDisplay()
{
    prepareSharedSpiBus();
    SPI.begin(BOARD_EPD_SCK, BOARD_SPI_MISO, BOARD_EPD_MOSI);
    display.init(115200, true, 2, false);
    display.setRotation(0);
    display.setTextColor(GxEPD_BLACK);
}

static bool initIoExpander()
{
    Wire.begin(BOARD_I2C_SDA, BOARD_I2C_SCL);
    if (!io.init(Wire, BOARD_I2C_SDA, BOARD_I2C_SCL, XL9555_SLAVE_ADDRESS0)) {
        return false;
    }
    io.configPort(ExtensionIOXL9555::PORT0, 0x00);
    io.configPort(ExtensionIOXL9555::PORT1, 0x00);
    io.digitalWrite(BOARD_XL9555_02_GPS_EN, HIGH);
    return true;
}

static bool gps_init_chip()
{
    SerialGPS.begin(38400, SERIAL_8N1, BOARD_GPS_RXD, BOARD_GPS_TXD);
    bool result = gps_recovery();
    if (!result) {
        SerialGPS.updateBaudRate(9600);
        result = gps_recovery();
        SerialGPS.updateBaudRate(38400);
    }
    return result;
}

void setup(void)
{
    SerialMon.begin(115200);
    delay(80);

    initDisplay();
    renderTick();

    g.xl9555_ok = initIoExpander();
    if (!g.xl9555_ok) {
        Serial.println("Failed to find XL9555 - check your wiring!");
    }

    g.gps_link_ok = gps_init_chip();
    if (!g.gps_link_ok) {
        Serial.println("GPS Connect failed~!");
    }

    delay(300);
    renderTick();
}

void loop(void)
{
    while (SerialMon.available()) {
        SerialGPS.write(SerialMon.read());
    }

    while (SerialGPS.available()) {
        int c = SerialGPS.read();
        g.last_data_at = millis();
        g.data_seen = true;
        if (gps.encode(c)) {
            refreshGpsState();
        }
    }

    if (millis() - g.last_render_at >= RENDER_INTERVAL_MS) {
        refreshGpsState();
        renderTick();
    }

    if (g.gps_link_ok && millis() > GPS_TIMEOUT_MS && gps.charsProcessed() < 10) {
        Serial.println(F("No GPS detected: check wiring."));
        delay(1000);
    }
}

static int getAck(uint8_t *buf, uint16_t size, uint8_t reqClass, uint8_t reqID)
{
    uint16_t frameCounter = 0;
    uint32_t startTime = millis();
    uint16_t needRead = 0;

    while (millis() - startTime < 800) {
        while (SerialGPS.available()) {
            int c = SerialGPS.read();
            switch (frameCounter) {
            case 0: if (c == 0xB5) frameCounter++; break;
            case 1: frameCounter = (c == 0x62) ? 2 : 0; break;
            case 2: frameCounter = (c == reqClass) ? 3 : 0; break;
            case 3: frameCounter = (c == reqID) ? 4 : 0; break;
            case 4: needRead = c; frameCounter++; break;
            case 5: needRead |= (c << 8); frameCounter++; break;
            case 6:
                if (needRead >= size) { frameCounter = 0; break; }
                if (SerialGPS.readBytes(buf, needRead) != needRead) frameCounter = 0;
                else return needRead;
                break;
            default: break;
            }
        }
    }
    return 0;
}

static bool gps_recovery()
{
    uint8_t cfg_clear1[] = {0xB5, 0x62, 0x06, 0x09, 0x0D, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x1C, 0xA2};
    uint8_t cfg_clear2[] = {0xB5, 0x62, 0x06, 0x09, 0x0D, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x1B, 0xA1};
    uint8_t cfg_clear3[] = {0xB5, 0x62, 0x06, 0x09, 0x0D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x03, 0x1D, 0xB3};
    SerialGPS.write(cfg_clear1, sizeof(cfg_clear1));
    getAck(buffer, 256, 0x05, 0x01);
    SerialGPS.write(cfg_clear2, sizeof(cfg_clear2));
    getAck(buffer, 256, 0x05, 0x01);
    SerialGPS.write(cfg_clear3, sizeof(cfg_clear3));
    getAck(buffer, 256, 0x05, 0x01);

    uint8_t cfg_rate[] = {0xB5, 0x62, 0x06, 0x08, 0x00, 0x00, 0x0E, 0x30};
    SerialGPS.write(cfg_rate, sizeof(cfg_rate));
    if (getAck(buffer, 256, 0x06, 0x08)) return true;
    return false;
}



