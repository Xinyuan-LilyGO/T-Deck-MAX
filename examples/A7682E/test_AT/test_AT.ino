

#include "Arduino.h"
#include <GxEPD2_BW.h>
#include <TDeckMaxBoard.h>
#include "ExtensionIOXL9555.hpp"

ExtensionIOXL9555 io;
bool reply = false;

GxEPD2_BW<GxEPD2_310_GDEQ031T10, GxEPD2_310_GDEQ031T10::HEIGHT> display(
    GxEPD2_310_GDEQ031T10(BOARD_EPD_CS, BOARD_EPD_DC, BOARD_EPD_RST, BOARD_EPD_BUSY));

void drawCenteredText(const char *text, int16_t y, uint8_t size)
{
    int16_t x1;
    int16_t y1;
    uint16_t width;
    uint16_t height;

    display.setTextSize(size);
    display.getTextBounds(text, 0, y, &x1, &y1, &width, &height);
    display.setCursor((display.width() - width) / 2 - x1, y);
    display.print(text);
}

void showStartupScreen()
{
    // LoRa, SD and EPD share one SPI bus; keep the unused devices deselected.
    pinMode(BOARD_LORA_CS, OUTPUT);
    digitalWrite(BOARD_LORA_CS, HIGH);
    pinMode(BOARD_LORA_RST, OUTPUT);
    digitalWrite(BOARD_LORA_RST, HIGH);
    pinMode(BOARD_SD_CS, OUTPUT);
    digitalWrite(BOARD_SD_CS, HIGH);
    pinMode(BOARD_EPD_CS, OUTPUT);
    digitalWrite(BOARD_EPD_CS, HIGH);

    pinMode(BOARD_EPD_BL, OUTPUT);
    analogWrite(BOARD_EPD_BL, 0);

    SPI.begin(BOARD_EPD_SCK, -1, BOARD_EPD_MOSI, BOARD_EPD_CS);
    display.init(115200, true, 2, false);
    display.setRotation(0);
    display.setTextColor(GxEPD_BLACK);
    display.setTextWrap(false);
    display.setFont(nullptr);
    display.setFullWindow();
    display.firstPage();
    do {
        display.fillScreen(GxEPD_WHITE);
        display.drawRect(12, 12, display.width() - 24, display.height() - 24, GxEPD_BLACK);
        display.drawRect(16, 16, display.width() - 32, display.height() - 32, GxEPD_BLACK);
        drawCenteredText("A7682E", 112, 4);
        drawCenteredText("AT TEST", 160, 3);
        drawCenteredText("DEVICE STARTED", 214, 2);
    } while (display.nextPage());
    display.powerOff();
}

void setup(void)
{
    SerialMon.begin(115200);
    SerialAT.begin(115200, SERIAL_8N1, BOARD_A7682E_TXD, BOARD_A7682E_RXD);
    // showStartupScreen();

    // XL9555 Init
    if (!io.init(Wire, BOARD_I2C_SDA, BOARD_I2C_SCL, XL9555_SLAVE_ADDRESS0)) {
        while (1) {
            Serial.println("Failed to find XL9555 - check your wiring!");
            delay(1000);
        }
    }
    io.configPort(ExtensionIOXL9555::PORT0, 0x00);      // Set PORT0 as output ,mask = 0x00 = all pin output
    io.configPort(ExtensionIOXL9555::PORT1, 0x00);      // Set PORT1 as output ,mask = 0x00 = all pin output
    io.digitalWrite(BOARD_XL9555_00_6609_EN, HIGH);     // enable A7682 module power
    io.digitalWrite(BOARD_XL9555_10_PWRKEY_EN, HIGH);   // power on
    io.digitalWrite(BOARD_XL9555_06_AMPLIFIER, HIGH);    // Enable power amplifier
    io.digitalWrite(BOARD_XL9555_12_AUDIO_SEL, HIGH);   // Audio output selection: A7682E module
    
    // A7682  Power on
    Serial.println("Power on the modem PWRKEY.");
    // A7682E_power_on();

    int i = 10;
    Serial.println("\nTesting Modem Response...\n");
    Serial.println("****");
    while (i)
    {
        SerialAT.println("AT");
        delay(500);
        if (SerialAT.available())
        {
            String r = SerialAT.readString();
            SerialAT.println(r);
            if (r.indexOf("OK") >= 0)
            {
                reply = true;
                break;
            }
        }
        delay(500);
        i--;
    }
    Serial.println("****\n");

    if (reply)
    {
        Serial.println(F("***********************************************************"));
        Serial.println(F(" You can now send AT commands"));
        Serial.println(F(" Enter \"AT\" (without quotes), and you should see \"OK\""));
        Serial.println(F(" If it doesn't work, select \"Both NL & CR\" in Serial Monitor"));
        Serial.println(F(" DISCLAIMER: Entering AT commands without knowing what they do"));
        Serial.println(F(" can have undesired consiquinces..."));
        Serial.println(F("***********************************************************\n"));
    }
    else
    {
        Serial.println(F("***********************************************************"));
        Serial.println(F(" Failed to connect to the modem! Check the baud and try again."));
        Serial.println(F("***********************************************************\n"));
    }
}

void loop(void)
{
    while (SerialAT.available())
    {
        SerialMon.write(SerialAT.read());
    }
    while (SerialMon.available())
    {
        SerialAT.write(SerialMon.read());
    }
}
