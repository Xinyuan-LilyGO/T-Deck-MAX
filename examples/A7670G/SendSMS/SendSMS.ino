// Migrated and adapted from LilyGo-Modem-Series/examples/SendSMS (MIT).
#include "../A7670GBoard.h"

static const char SMS_TARGET[] = "+8613800138000"; // Include country code.
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
    if (!modem.waitForNetwork(120000L, true)) {
        SerialMon.println("Network registration failed.");
        return;
    }

    const String message = String("Hello from T-Deck Pro MAX, IMEI ") + modem.getIMEI();
    SerialMon.print("Sending SMS to ");
    SerialMon.println(SMS_TARGET);
    SerialMon.println(modem.sendSMS(SMS_TARGET, message) ? "SMS sent." : "SMS send failed.");
}

void loop()
{
    delay(1000);
}
