// Migrated and adapted from LilyGo-Modem-Series/examples/ATdebug (MIT).
#include "../A7670GBoard.h"

TinyGsm modem(SerialAT);

void setup()
{
    if (!A7670GBoard::begin(modem, true)) {
        SerialMon.println("Modem did not respond. Check the battery and LTE antenna.");
        return;
    }
    SerialMon.println("A7670G is online. Set the monitor to Both NL & CR and enter AT commands.");
}

void loop()
{
    A7670GBoard::serialBridge();
    delay(1);
}
