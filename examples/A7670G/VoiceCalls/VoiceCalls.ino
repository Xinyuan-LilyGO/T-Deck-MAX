// Migrated and adapted from LilyGo-Modem-Series/examples/VoiceCalls (MIT).
#include "../A7670GBoard.h"

static const char PHONE_NUMBER[] = "+8613800138000"; // Include country code.
static const uint32_t CALL_DURATION_MS = 20000;
TinyGsm modem(SerialAT);

void setup()
{
    if (!A7670GBoard::begin(modem, true)) {
        SerialMon.println("Modem startup failed.");
        return;
    }
    if (!modem.waitForNetwork(120000L, true)) {
        SerialMon.println("Network registration failed.");
        return;
    }

    SerialMon.print("Calling ");
    SerialMon.println(PHONE_NUMBER);
    if (!modem.callNumber(PHONE_NUMBER)) {
        SerialMon.println("Call setup failed or the call was rejected.");
        return;
    }
    delay(CALL_DURATION_MS);
    modem.callHangup();
    SerialMon.println("Call ended.");
}

void loop()
{
    A7670GBoard::serialBridge();
    delay(1);
}
