#pragma once

// Shared T-Deck Pro MAX hardware support for the migrated A7670G examples.
// Adapted from LilyGo-Modem-Series; board power control is specific to this
// project and intentionally replaces the source repository's utilities.h.

#ifdef TINY_GSM_MODEM_SIM7672
#undef TINY_GSM_MODEM_SIM7672
#endif
#ifndef TINY_GSM_MODEM_A7670
#define TINY_GSM_MODEM_A7670
#endif
#ifndef TINY_GSM_RX_BUFFER
#define TINY_GSM_RX_BUFFER 1024
#endif

#include <Arduino.h>
#include <Wire.h>
#include <TDeckMaxBoard.h>
#include <ExtensionIOXL9555.hpp>
#include <TinyGsmClient.h>

namespace A7670GBoard {

static ExtensionIOXL9555 io;
static bool ioReady = false;

static bool beginHardware(bool routeModemAudio = false)
{
    SerialMon.begin(115200);
    delay(100);

    pinMode(BOARD_A7682E_DTR, OUTPUT);
    digitalWrite(BOARD_A7682E_DTR, LOW);
    pinMode(BOARD_A7682E_RI, INPUT_PULLUP);

    SerialAT.setRxBufferSize(4096);
    SerialAT.begin(115200, SERIAL_8N1, BOARD_A7682E_TXD, BOARD_A7682E_RXD);

    if (!io.init(Wire, BOARD_I2C_SDA, BOARD_I2C_SCL, XL9555_SLAVE_ADDRESS0)) {
        SerialMon.println("XL9555 was not found; modem power control is unavailable.");
        return false;
    }
    ioReady = true;

    io.pinMode(BOARD_XL9555_00_6609_EN, OUTPUT);
    io.pinMode(BOARD_XL9555_10_PWRKEY_EN, OUTPUT);
    io.pinMode(BOARD_XL9555_06_AMPLIFIER, OUTPUT);
    io.pinMode(BOARD_XL9555_12_AUDIO_SEL, OUTPUT);

    io.digitalWrite(BOARD_XL9555_10_PWRKEY_EN, LOW);
    io.digitalWrite(BOARD_XL9555_00_6609_EN, HIGH);
    io.digitalWrite(BOARD_XL9555_06_AMPLIFIER, routeModemAudio ? HIGH : LOW);
    io.digitalWrite(BOARD_XL9555_12_AUDIO_SEL, routeModemAudio ? HIGH : LOW);
    delay(1500);
    return true;
}

static void powerOnPulse()
{
    if (!ioReady) {
        return;
    }
    io.digitalWrite(BOARD_XL9555_10_PWRKEY_EN, LOW);
    delay(100);
    io.digitalWrite(BOARD_XL9555_10_PWRKEY_EN, HIGH);
    delay(1100);
    io.digitalWrite(BOARD_XL9555_10_PWRKEY_EN, LOW);
}

static void setPowerRail(bool enabled)
{
    if (ioReady) {
        io.digitalWrite(BOARD_XL9555_00_6609_EN, enabled ? HIGH : LOW);
    }
}

static void routeAudio(bool enabled)
{
    if (!ioReady) {
        return;
    }
    io.digitalWrite(BOARD_XL9555_12_AUDIO_SEL, enabled ? HIGH : LOW);
    io.digitalWrite(BOARD_XL9555_06_AMPLIFIER, enabled ? HIGH : LOW);
}

static bool waitForModem(TinyGsm &modem, uint32_t timeoutMs = 35000)
{
    if (modem.testAT(1500)) {
        return true;
    }

    SerialMon.println("Pulsing modem PWRKEY...");
    powerOnPulse();
    const uint32_t start = millis();
    while (millis() - start < timeoutMs) {
        if (modem.testAT(1000)) {
            return true;
        }
        SerialMon.print('.');
        delay(500);
    }
    SerialMon.println();
    return false;
}

static bool begin(TinyGsm &modem, bool routeModemAudio = false)
{
    return beginHardware(routeModemAudio) && waitForModem(modem);
}

static bool connectData(TinyGsm &modem, const char *apn,
                        const char *user = "", const char *password = "",
                        const char *simPin = "", uint32_t timeoutMs = 120000)
{
    if (simPin && simPin[0] && modem.getSimStatus() == SIM_LOCKED) {
        if (!modem.simUnlock(simPin)) {
            SerialMon.println("SIM unlock failed.");
            return false;
        }
    }

    SerialMon.print("Waiting for cellular network...");
    if (!modem.waitForNetwork(timeoutMs, true)) {
        SerialMon.println(" failed");
        return false;
    }
    SerialMon.println(" connected");

    SerialMon.print("Activating data context with APN: ");
    SerialMon.println(apn);
    if (!modem.gprsConnect(apn, user, password)) {
        SerialMon.println("Data context activation failed.");
        return false;
    }
    SerialMon.print("IP address: ");
    SerialMon.println(modem.getLocalIP());
    return true;
}

static void serialBridge()
{
    while (SerialAT.available()) {
        SerialMon.write(SerialAT.read());
    }
    while (SerialMon.available()) {
        SerialAT.write(SerialMon.read());
    }
}

static bool beginExternalGps(uint32_t baud = 9600)
{
    SerialMon.begin(115200);
    delay(100);
    if (!io.init(Wire, BOARD_I2C_SDA, BOARD_I2C_SCL, XL9555_SLAVE_ADDRESS0)) {
        SerialMon.println("XL9555 was not found; GPS power control is unavailable.");
        return false;
    }
    ioReady = true;
    io.pinMode(BOARD_XL9555_02_GPS_EN, OUTPUT);
    io.digitalWrite(BOARD_XL9555_02_GPS_EN, HIGH);
    SerialGPS.setRxBufferSize(2048);
    SerialGPS.begin(baud, SERIAL_8N1, BOARD_GPS_RXD, BOARD_GPS_TXD);
    delay(500);
    return true;
}

} // namespace A7670GBoard
