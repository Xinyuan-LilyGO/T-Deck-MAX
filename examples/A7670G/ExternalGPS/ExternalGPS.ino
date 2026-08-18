// Adapted from LilyGo-Modem-Series/examples/ExternalGPS_A7670G_Only (MIT).
// T-Deck Pro MAX uses its independent onboard MIA-M10Q receiver here.
#include "../A7670GBoard.h"
#include <TinyGPS++.h>

TinyGPSPlus gps;

static void printFix()
{
    SerialMon.print("Location: ");
    if (gps.location.isValid()) {
        SerialMon.print(gps.location.lat(), 6);
        SerialMon.print(',');
        SerialMon.print(gps.location.lng(), 6);
    } else {
        SerialMon.print("INVALID");
    }
    SerialMon.print("  Satellites: ");
    SerialMon.print(gps.satellites.isValid() ? String(gps.satellites.value()) : "INVALID");
    SerialMon.print("  UTC: ");
    if (gps.date.isValid() && gps.time.isValid()) {
        SerialMon.printf("%04d-%02d-%02d %02d:%02d:%02d",
                         gps.date.year(), gps.date.month(), gps.date.day(),
                         gps.time.hour(), gps.time.minute(), gps.time.second());
    } else {
        SerialMon.print("INVALID");
    }
    SerialMon.println();
}

void setup()
{
    if (!A7670GBoard::beginExternalGps()) {
        return;
    }
    SerialMon.println("Waiting for NMEA data from the onboard MIA-M10Q...");
}

void loop()
{
    while (SerialGPS.available()) {
        if (gps.encode(SerialGPS.read())) {
            printFix();
        }
    }
    if (millis() > 30000 && gps.charsProcessed() < 10) {
        SerialMon.println("No GPS data: verify antenna view and GPS power.");
        delay(1000);
    }
}
