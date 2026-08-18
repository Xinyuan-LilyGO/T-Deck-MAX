// Migrated and adapted from LilyGo-Modem-Series/examples/TCPClientMultiple (MIT).
#include "../A7670GBoard.h"

static const char APN[] = "YourAPN";
static const char GPRS_USER[] = "";
static const char GPRS_PASSWORD[] = "";

TinyGsm modem(SerialAT);
TinyGsmClient client0(modem, 0);
TinyGsmClient client1(modem, 1);

static void request(TinyGsmClient &client, const char *host, const char *resource)
{
    SerialMon.print("Connecting to ");
    SerialMon.println(host);
    if (!client.connect(host, 80)) {
        SerialMon.println("Connection failed.");
        return;
    }
    client.print(String("GET ") + resource + " HTTP/1.0\r\nHost: " + host +
                 "\r\nConnection: close\r\n\r\n");
}

void setup()
{
    if (!A7670GBoard::begin(modem) ||
        !A7670GBoard::connectData(modem, APN, GPRS_USER, GPRS_PASSWORD)) {
        SerialMon.println("Modem or data connection setup failed.");
        return;
    }
    request(client0, "vsh.pp.ua", "/TinyGSM/logo.txt");
    request(client1, "httpbin.org", "/get");

    const uint32_t start = millis();
    while (millis() - start < 30000L &&
           (client0.connected() || client1.connected() || client0.available() || client1.available())) {
        while (client0.available()) {
            SerialMon.write(client0.read());
        }
        while (client1.available()) {
            SerialMon.write(client1.read());
        }
        delay(10);
    }
    client0.stop();
    client1.stop();
    SerialMon.println("\nBoth socket requests are complete.");
}

void loop()
{
    delay(1000);
}
