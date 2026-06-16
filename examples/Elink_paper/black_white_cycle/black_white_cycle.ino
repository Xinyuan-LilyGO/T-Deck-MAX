#include <Arduino.h>
#include <GxEPD2_BW.h>

#include "utilities.h"

GxEPD2_BW<GxEPD2_310_GDEQ031T10, GxEPD2_310_GDEQ031T10::HEIGHT> display(
    GxEPD2_310_GDEQ031T10(BOARD_EPD_CS, BOARD_EPD_DC, BOARD_EPD_RST, BOARD_EPD_BUSY));

static constexpr uint32_t kHoldTimeMs = 2000;

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

void drawSolidScreen(uint16_t color)
{
    display.setFullWindow();
    display.firstPage();
    do {
        display.fillScreen(color);
    } while (display.nextPage());
}

void turnOffFrontlight()
{
    pinMode(BOARD_EPD_BL, OUTPUT);
    analogWrite(BOARD_EPD_BL, 0);
}

void setup()
{
    Serial.begin(115200);
    delay(100);

    prepareSharedSpiBus();
    turnOffFrontlight();

    SPI.begin(BOARD_EPD_SCK, -1, BOARD_EPD_MOSI, BOARD_EPD_CS);
    display.init(115200, true, 2, false);

    Serial.println("EPD black/white cycle example started");
}

void loop()
{
    drawSolidScreen(GxEPD_BLACK);
    delay(kHoldTimeMs);

    drawSolidScreen(GxEPD_WHITE);
    delay(kHoldTimeMs);
}
