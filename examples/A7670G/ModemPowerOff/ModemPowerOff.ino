// Migrated and adapted from LilyGo-Modem-Series/examples/ModemPowerOff (MIT).
#include "../A7670GBoard.h"

TinyGsm modem(SerialAT);

void setup()
{
    if (!A7670GBoard::begin(modem)) {
        SerialMon.println("Modem startup failed.");
        return;
    }
    SerialMon.println("Requesting orderly modem shutdown...");
    if (!modem.poweroff()) {
        SerialMon.println("AT+CPOF failed.");
        return;
    }
    delay(5000);
    A7670GBoard::setPowerRail(false);
    SerialMon.println("Modem is off and its power rail is disabled. Reset the board to restart it.");
}

void loop()
{
    delay(1000);
}
