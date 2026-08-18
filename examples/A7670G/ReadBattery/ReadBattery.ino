// Migrated and adapted from LilyGo-Modem-Series/examples/ReadBattery (MIT).
#include "../A7670GBoard.h"

TinyGsm modem(SerialAT);

void setup()
{
    if (!A7670GBoard::begin(modem)) {
        SerialMon.println("Modem startup failed.");
        return;
    }
    SerialMon.println("Reading A7670G supply voltage with AT+CBC.");
}

void loop()
{
    const uint16_t millivolts = modem.getBattVoltage();
    if (millivolts) {
        SerialMon.printf("Modem VBAT: %u mV\n", millivolts);
    } else {
        SerialMon.println("AT+CBC did not return a voltage on this modem/firmware.");
    }
    delay(5000);
}
