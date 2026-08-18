// Migrated and adapted from LilyGo-Modem-Series/examples/HttpsClient (MIT).
#include "../A7670GBoard.h"

static const char APN[] = "YourAPN";
static const char GPRS_USER[] = "";
static const char GPRS_PASSWORD[] = "";
static const char URL[] = "https://httpbin.org/get";
TinyGsm modem(SerialAT);

void setup()
{
    if (!A7670GBoard::begin(modem) ||
        !A7670GBoard::connectData(modem, APN, GPRS_USER, GPRS_PASSWORD)) {
        SerialMon.println("Modem or data connection setup failed.");
        return;
    }
    if (!modem.https_begin() || !modem.https_set_url(URL)) {
        SerialMon.println("HTTPS service initialization failed.");
        return;
    }

    size_t bodyLength = 0;
    const int status = modem.https_get(&bodyLength);
    SerialMon.printf("HTTP status: %d, body length: %u\n", status, bodyLength);
    if (status > 0) {
        SerialMon.println(modem.https_header());
        SerialMon.println(modem.https_body());
    }
    modem.https_end();
}

void loop()
{
    delay(1000);
}
