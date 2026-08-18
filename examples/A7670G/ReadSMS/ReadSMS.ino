// Migrated and adapted from LilyGo-Modem-Series/examples/ReadSMS (MIT).
#include "../A7670GBoard.h"

static const char SIM_PIN[] = "";
TinyGsm modem(SerialAT);

static void listMessages()
{
    String response;
    modem.sendAT("+CMGF=1");
    if (modem.waitResponse(10000L) != 1) {
        SerialMon.println("Could not select SMS text mode.");
        return;
    }
    modem.sendAT("+CMGL=\"ALL\"");
    if (modem.waitResponse(15000L, response) == 1) {
        response.replace("\r\nOK\r\n", "");
        response.trim();
        SerialMon.println(response.length() ? response : "No stored SMS messages.");
    } else {
        SerialMon.println("Could not list SMS messages.");
    }
}

void setup()
{
    if (!A7670GBoard::begin(modem)) {
        SerialMon.println("Modem startup failed.");
        return;
    }
    if (SIM_PIN[0] && modem.getSimStatus() == SIM_LOCKED) {
        modem.simUnlock(SIM_PIN);
    }
    if (!modem.waitForNetwork(120000L, true)) {
        SerialMon.println("Network registration failed.");
        return;
    }
    listMessages();
    SerialMon.println("AT bridge active; use AT+CMGR=<index> to read one message.");
}

void loop()
{
    A7670GBoard::serialBridge();
    delay(1);
}
