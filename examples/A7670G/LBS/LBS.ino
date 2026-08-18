// Migrated and adapted from LilyGo-Modem-Series/examples/LBSExample (MIT).
#include "../A7670GBoard.h"

static const char APN[] = "YourAPN";
static const char GPRS_USER[] = "";
static const char GPRS_PASSWORD[] = "";
TinyGsm modem(SerialAT);

void setup()
{
    if (!A7670GBoard::begin(modem) ||
        !A7670GBoard::connectData(modem, APN, GPRS_USER, GPRS_PASSWORD)) {
        SerialMon.println("Modem or data connection setup failed.");
    }
}

void loop()
{
    float latitude = 0;
    float longitude = 0;
    float accuracy = 0;
    int year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0;

    SerialMon.println("Requesting cell-based location...");
    if (modem.getGsmLocation(&latitude, &longitude, &accuracy,
                             &year, &month, &day, &hour, &minute, &second)) {
        SerialMon.printf("Location: %.6f, %.6f; accuracy %.0f m\n",
                         latitude, longitude, accuracy);
        SerialMon.printf("UTC: %04d-%02d-%02d %02d:%02d:%02d\n",
                         year, month, day, hour, minute, second);
    } else {
        SerialMon.println("LBS request failed; retrying in 30 seconds.");
    }
    delay(30000);
}
