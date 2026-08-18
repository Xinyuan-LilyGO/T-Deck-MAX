// Migrated and adapted from LilyGo-Modem-Series/examples/TextToSpeech (MIT).
#include "../A7670GBoard.h"

static const char TEXT_TO_SPEAK[] = "Welcome to LilyGO T Deck Pro Max";
TinyGsm modem(SerialAT);

void setup()
{
    if (!A7670GBoard::begin(modem, true)) {
        SerialMon.println("Modem startup failed.");
        return;
    }

    modem.sendAT("+CTTSPARAM=1,3,0,1,1,0");
    if (modem.waitResponse(10000L) != 1) {
        SerialMon.println("TTS parameter setup failed; check modem firmware support.");
        return;
    }
    modem.sendAT("+CTTS=2,\"", TEXT_TO_SPEAK, "\"");
    if (modem.waitResponse(10000L) != 1) {
        SerialMon.println("TTS start failed.");
        return;
    }
    SerialMon.println("Speaking through the modem audio path...");
    if (modem.waitResponse(120000L, "+CTTS: 0") != 1) {
        SerialMon.println("TTS completion indication was not received.");
    } else {
        SerialMon.println("TTS complete.");
    }
}

void loop()
{
    A7670GBoard::serialBridge();
    delay(1);
}
