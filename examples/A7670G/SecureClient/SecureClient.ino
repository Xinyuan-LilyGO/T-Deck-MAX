// Migrated and adapted from LilyGo-Modem-Series/examples/SecureClient (MIT).
#include "../A7670GBoard.h"

static const char APN[] = "YourAPN";
static const char GPRS_USER[] = "";
static const char GPRS_PASSWORD[] = "";
static const char URL[] = "https://httpbin.org/headers";
TinyGsm modem(SerialAT);

void setup()
{
    if (!A7670GBoard::begin(modem) ||
        !A7670GBoard::connectData(modem, APN, GPRS_USER, GPRS_PASSWORD)) {
        SerialMon.println("Modem or data connection setup failed.");
        return;
    }

    // TLS 1.2 with encrypted transport. authmode=0 does not validate a CA;
    // install a CA certificate and use authmode=1 for authenticated production TLS.
    modem.sendAT("+CSSLCFG=\"sslversion\",0,3");
    modem.waitResponse(10000L);
    modem.sendAT("+CSSLCFG=\"authmode\",0,0");
    modem.waitResponse(10000L);

    if (!modem.https_begin() || !modem.https_set_url(URL)) {
        SerialMon.println("Secure HTTP service initialization failed.");
        return;
    }
    modem.https_set_user_agent("T-Deck-Pro-MAX-A7670G/1.0");
    size_t bodyLength = 0;
    const int status = modem.https_get(&bodyLength);
    SerialMon.printf("HTTPS status: %d, body length: %u\n", status, bodyLength);
    if (status > 0) {
        SerialMon.println(modem.https_body());
    }
    modem.https_end();
}

void loop()
{
    delay(1000);
}
