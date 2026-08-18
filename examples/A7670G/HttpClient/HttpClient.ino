// Migrated and adapted from LilyGo-Modem-Series/examples/HttpClient (MIT).
#include "../A7670GBoard.h"

static const char APN[] = "YourAPN";
static const char GPRS_USER[] = "";
static const char GPRS_PASSWORD[] = "";
static const char SERVER[] = "vsh.pp.ua";
static const char RESOURCE[] = "/TinyGSM/logo.txt";
static const uint16_t PORT = 80;

TinyGsm modem(SerialAT);
TinyGsmClient client(modem);

void setup()
{
    if (!A7670GBoard::begin(modem) ||
        !A7670GBoard::connectData(modem, APN, GPRS_USER, GPRS_PASSWORD)) {
        SerialMon.println("Modem or data connection setup failed.");
        return;
    }
    if (!client.connect(SERVER, PORT)) {
        SerialMon.println("TCP connection failed.");
        return;
    }

    client.print(String("GET ") + RESOURCE + " HTTP/1.1\r\nHost: " + SERVER +
                 "\r\nConnection: close\r\n\r\n");
    uint32_t lastData = millis();
    while ((client.connected() || client.available()) && millis() - lastData < 30000L) {
        while (client.available()) {
            SerialMon.write(client.read());
            lastData = millis();
        }
        delay(10);
    }
    client.stop();
    SerialMon.println("\nHTTP request complete.");
}

void loop()
{
    delay(1000);
}
