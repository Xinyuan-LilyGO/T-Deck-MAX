// Migrated and adapted from LilyGo-Modem-Series/examples/ModemSleep (MIT).
#include "../A7670GBoard.h"
#include <driver/gpio.h>
#include <esp_sleep.h>

static const uint32_t SLEEP_SECONDS = 30;
TinyGsm modem(SerialAT);

void setup()
{
    const bool timerWake = esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER;
    if (timerWake) {
        gpio_hold_dis((gpio_num_t)BOARD_A7682E_DTR);
    }

    if (!A7670GBoard::beginHardware()) {
        return;
    }

    if (timerWake) {
        digitalWrite(BOARD_A7682E_DTR, LOW);
        delay(300);
        modem.sleepEnable(false);
        SerialMon.println(A7670GBoard::waitForModem(modem, 10000) ?
                          "Modem woke from sleep." : "Modem wake failed.");
        delay(3000);
    } else if (!A7670GBoard::waitForModem(modem)) {
        SerialMon.println("Modem startup failed.");
        return;
    }

    if (!modem.sleepEnable(true)) {
        SerialMon.println("AT+CSCLK sleep enable failed.");
        return;
    }
    digitalWrite(BOARD_A7682E_DTR, HIGH);
    gpio_hold_en((gpio_num_t)BOARD_A7682E_DTR);
    gpio_deep_sleep_hold_en();

    SerialMon.printf("Modem and ESP32-S3 sleeping for %u seconds.\n", SLEEP_SECONDS);
    SerialMon.flush();
    esp_sleep_enable_timer_wakeup((uint64_t)SLEEP_SECONDS * 1000000ULL);
    delay(100);
    esp_deep_sleep_start();
}

void loop()
{
}
