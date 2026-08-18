// Migrated and adapted from LilyGo-Modem-Series/examples/DeepSleep (MIT).
#include "../A7670GBoard.h"
#include <esp_sleep.h>

static const uint32_t SLEEP_SECONDS = 30;
TinyGsm modem(SerialAT);

void setup()
{
    if (!A7670GBoard::begin(modem)) {
        SerialMon.println("Modem startup failed.");
        return;
    }
    SerialMon.println("Powering off modem before ESP32-S3 deep sleep...");
    if (!modem.poweroff()) {
        SerialMon.println("AT+CPOF failed; deep sleep cancelled.");
        return;
    }
    delay(3000);
    A7670GBoard::setPowerRail(false);
    SerialMon.printf("Sleeping for %u seconds.\n", SLEEP_SECONDS);
    SerialMon.flush();
    esp_sleep_enable_timer_wakeup((uint64_t)SLEEP_SECONDS * 1000000ULL);
    delay(100);
    esp_deep_sleep_start();
}

void loop()
{
}
