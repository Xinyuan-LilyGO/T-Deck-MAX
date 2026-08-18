// Migrated and adapted from LilyGo-Modem-Series/examples/Network (MIT).
#include "../A7670GBoard.h"

static const char SIM_PIN[] = "";
TinyGsm modem(SerialAT);

void setup()
{
    if (!A7670GBoard::begin(modem)) {
        SerialMon.println("Modem startup failed.");
        return;
    }

    if (SIM_PIN[0] && modem.getSimStatus() == SIM_LOCKED) {
        modem.simUnlock(SIM_PIN);
    }

    SerialMon.print("Modem: ");
    SerialMon.println(modem.getModemInfo());
    SerialMon.print("IMEI: ");
    SerialMon.println(modem.getIMEI());
    SerialMon.print("SIM CCID: ");
    SerialMon.println(modem.getSimCCID());
    SerialMon.print("Waiting for network...");
    if (!modem.waitForNetwork(120000L, true)) {
        SerialMon.println(" failed");
        return;
    }
    SerialMon.println(" connected");
    SerialMon.print("Operator: ");
    SerialMon.println(modem.getOperator());
    SerialMon.print("Signal quality (CSQ): ");
    SerialMon.println(modem.getSignalQuality());
    SerialMon.print("Network mode: ");
    SerialMon.println(modem.getNetworkModes());
}

void loop()
{
    SerialMon.printf("Registered: %s, CSQ: %d\n",
                     modem.isNetworkConnected() ? "yes" : "no",
                     modem.getSignalQuality());
    delay(5000);
}
