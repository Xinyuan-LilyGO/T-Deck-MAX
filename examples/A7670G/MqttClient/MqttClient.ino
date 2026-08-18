// Migrated and adapted from LilyGo-Modem-Series/examples/MqttClient (MIT).
#include "../A7670GBoard.h"

static const char APN[] = "YourAPN";
static const char GPRS_USER[] = "";
static const char GPRS_PASSWORD[] = "";
static const char BROKER[] = "broker.emqx.io";
static const uint16_t BROKER_PORT = 1883;
static const char CLIENT_ID[] = "tdeck-pro-max-a7670g";
static const char TOPIC_RX[] = "lilygo/tdeck-pro-max/rx";
static const char TOPIC_TX[] = "lilygo/tdeck-pro-max/tx";

TinyGsm modem(SerialAT);

static void onMessage(const char *topic, const uint8_t *payload, uint32_t length)
{
    SerialMon.print("MQTT [");
    SerialMon.print(topic);
    SerialMon.print("]: ");
    SerialMon.write(payload, length);
    SerialMon.println();
}

void setup()
{
    if (!A7670GBoard::begin(modem) ||
        !A7670GBoard::connectData(modem, APN, GPRS_USER, GPRS_PASSWORD)) {
        SerialMon.println("Modem or data connection setup failed.");
        return;
    }
    modem.mqtt_set_callback(onMessage);
    if (!modem.mqtt_begin(false) ||
        !modem.mqtt_connect(0, BROKER, BROKER_PORT, CLIENT_ID) ||
        !modem.mqtt_subscribe(0, TOPIC_RX, 0)) {
        SerialMon.println("MQTT setup failed.");
        return;
    }
    modem.mqtt_publish(0, TOPIC_TX, "T-Deck Pro MAX connected", 0);
    SerialMon.println("MQTT connected.");
}

void loop()
{
    modem.mqtt_handle(500);
    delay(10);
}
