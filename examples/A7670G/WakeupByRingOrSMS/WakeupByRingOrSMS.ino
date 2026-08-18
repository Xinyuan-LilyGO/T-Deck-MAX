// Migrated and adapted from LilyGo-Modem-Series/examples/WakeupByRingOrSMS (MIT).
#include "../A7670GBoard.h"
#include <driver/gpio.h>
#include <esp_sleep.h>

TinyGsm modem(SerialAT);

static void printWakeReason()
{
    const esp_sleep_wakeup_cause_t reason = esp_sleep_get_wakeup_cause();
    SerialMon.print("Wake cause: ");
    SerialMon.println(reason == ESP_SLEEP_WAKEUP_EXT0 ? "modem RI" : "power-on/reset");
}

void setup()
{
    if (!A7670GBoard::begin(modem)) {
        SerialMon.println("Modem startup failed.");
        return;
    }
    printWakeReason();

    if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT0) {
        SerialMon.println("RI asserted. Incoming call/SMS details follow on the AT bridge.");
        const uint32_t until = millis() + 5000;
        while ((int32_t)(until - millis()) > 0) {
            A7670GBoard::serialBridge();
            delay(1);
        }
    } else if (!modem.waitForNetwork(120000L, true)) {
        SerialMon.println("Network registration failed.");
        return;
    }

    pinMode(BOARD_A7682E_RI, INPUT_PULLUP);
    gpio_pullup_en((gpio_num_t)BOARD_A7682E_RI);
    while (digitalRead(BOARD_A7682E_RI) == LOW) {
        delay(20);
    }
    SerialMon.println("Sleeping. An incoming call or SMS can pull RI low and wake the ESP32-S3.");
    SerialMon.flush();
    esp_sleep_enable_ext0_wakeup((gpio_num_t)BOARD_A7682E_RI, 0);
    delay(100);
    esp_deep_sleep_start();
}

void loop()
{
}
