

#include <RadioLib.h>
#include "utilities.h"
#include "peripheral.h"
#include "ui_deckpro_port.h"


static SX1262 radio = new Module(BOARD_LORA_CS, BOARD_LORA_INT, BOARD_LORA_RST, BOARD_LORA_BUSY);
static bool lora_available = false;
static int lora_mode = LORA_MODE_SEND;
static String lora_recv_data;
static bool lora_recv_success = false;
static int lora_recv_rssi = 0;

// transmit 
static int transmissionState = RADIOLIB_ERR_NONE;
static volatile bool transmittedFlag = false;

static void set_transmit_flag(void){
    transmittedFlag = true;
}

// receive
static int receivedState = RADIOLIB_ERR_NONE;
static volatile bool receivedFlag = false;

static void set_receive_flag(void){
    receivedFlag = true;
}

static bool lora_state_ok(const char *operation, int state)
{
    if (state == RADIOLIB_ERR_NONE) {
        return true;
    }

    Serial.print(F("[SX1262] "));
    Serial.print(operation);
    Serial.print(F(" failed, code "));
    Serial.println(state);
    return false;
}

bool lora_init(void)
{
    lora_available = false;
    transmittedFlag = false;
    receivedFlag = false;
    lora_recv_success = false;
    lora_mode = LORA_MODE_SEND;

    Serial.print(F("[SX1262] Initializing ... "));
    int state = radio.begin(ui_lora_get_freq());
    if (state == RADIOLIB_ERR_NONE) {
        Serial.println(F("success!"));
    } else {
        Serial.print(F("failed, code "));
        Serial.println(state);
        return false;
    }

    radio.setPacketSentAction(set_transmit_flag);

    // set carrier frequency to 433.5 MHz
    state = radio.setFrequency(ui_lora_get_freq());
    if (!lora_state_ok("set frequency", state)) {
        return false;
    }

    // set bandwidth to 250 kHz
    state = radio.setBandwidth(ui_lora_get_bandwidth());
    if (!lora_state_ok("set bandwidth", state)) {
        return false;
    }

    // set spreading factor to 10
    state = radio.setSpreadingFactor(10);
    if (!lora_state_ok("set spreading factor", state)) {
        return false;
    }

    // set coding rate to 6
    state = radio.setCodingRate(6);
    if (!lora_state_ok("set coding rate", state)) {
        return false;
    }

    // set LoRa sync word to 0xAB
    state = radio.setSyncWord(0xAB);
    if (!lora_state_ok("set sync word", state)) {
        return false;
    }

    // set output power to 10 dBm (accepted range is -17 - 22 dBm)
    state = radio.setOutputPower(ui_lora_get_power());
    if (!lora_state_ok("set output power", state)) {
        return false;
    }

    // set over current protection limit to 80 mA (accepted range is 45 - 240 mA)
    // NOTE: set value to 0 to disable overcurrent protection
    state = radio.setCurrentLimit(140);
    if (!lora_state_ok("set current limit", state)) {
        return false;
    }

    // set LoRa preamble length to 15 symbols (accepted range is 0 - 65535)
    state = radio.setPreambleLength(15);
    if (!lora_state_ok("set preamble length", state)) {
        return false;
    }

    // disable CRC
    state = radio.setCRC(false);
    if (!lora_state_ok("set CRC", state)) {
        return false;
    }

    // Some SX126x modules have TCXO (temperature compensated crystal
    // oscillator). To configure TCXO reference voltage,
    // the following method can be used.
    state = radio.setTCXO(2.4);
    if (!lora_state_ok("set TCXO", state)) {
        return false;
    }

    // Some SX126x modules use DIO2 as RF switch. To enable
    // this feature, the following method can be used.
    // NOTE: As long as DIO2 is configured to control RF switch,
    //       it can't be used as interrupt pin!
    state = radio.setDio2AsRfSwitch();
    if (!lora_state_ok("set DIO2 RF switch", state)) {
        return false;
    }

    Serial.println(F("All settings succesfully changed!"));

    Serial.println(F("[SX1262] Sending first packet ... "));
    transmissionState = radio.startTransmit("Hello World!");
    if (!lora_state_ok("start transmit", transmissionState)) {
        return false;
    }

    lora_available = true;

    return true;
}

bool lora_is_available(void)
{
    return lora_available;
}

void lora_set_mode(int mode) 
{
    if (!lora_available) {
        return;
    }

    if(mode == LORA_MODE_SEND){
        radio.setPacketSentAction(set_transmit_flag);
        Serial.println(F("[LORA] Sending first packet ... "));
        transmissionState = radio.startTransmit("Hello World!");
        lora_state_ok("start transmit", transmissionState);
    } else if(mode == LORA_MODE_RECV){
        radio.setPacketReceivedAction(set_receive_flag);
        Serial.println(F("[LORA] Starting to listen ... "));
        receivedState = radio.startReceive();
        if (lora_state_ok("start receive", receivedState)) {
            Serial.println(F("success!"));
        }
    } else {
        return;
    }
    lora_mode = mode;
}

int lora_get_mode(void)
{
    return lora_mode;
}

void lora_receive_loop(void)
{
    if (!lora_available) {
        return;
    }

    if(receivedFlag){
        receivedFlag = false;

        lora_recv_success = true;

        // String str;
        receivedState = radio.readData(lora_recv_data);
        if(receivedState == RADIOLIB_ERR_NONE){
            Serial.println(F("[SX1262] Received packet!"));

            Serial.print(F("[SX1262] Data:\t\t"));
            Serial.println(lora_recv_data.c_str());
            // Serial.println("%d", (int)lora_recv_data.toInt());

            Serial.print(F("[SX1262] RSSI:\t\t"));
            Serial.print(radio.getRSSI());
            lora_recv_rssi = (int) radio.getRSSI();
            Serial.println(F(" dBm"));
        }else{
            Serial.print(F("failed, code "));
            Serial.println(receivedState);
        }
    }
}

void lora_transmit(const char *str)
{
    if (!lora_available || str == NULL) {
        return;
    }

    if(transmittedFlag){
        transmittedFlag = false;
        if(transmissionState == RADIOLIB_ERR_NONE){
            Serial.println(F("transmission finished!"));
        } else {
            Serial.print(F("failed, code "));
            Serial.println(transmissionState);
        }

        radio.finishTransmit();
        Serial.print(F("[Lora] Sending another packet ... "));
        transmissionState = radio.startTransmit(str);
        lora_state_ok("start transmit", transmissionState);
    }
}

bool lora_get_recv(const char **str, int *rssi)
{
    if (str != NULL) {
        *str = lora_recv_data.c_str();
    }
    if (rssi != NULL) {
        *rssi = lora_recv_rssi;
    }
    return lora_available && lora_recv_success;
}

void lora_set_recv_flag(void)
{
    lora_recv_success = false;
}

void lora_sleep(void)
{
    if (lora_available) {
        radio.sleep();
    }
}

void lora_param_set(void)
{
    if (!lora_available) {
        return;
    }

    // set carrier frequency to 433.5 MHz
    int state = radio.setFrequency(ui_lora_get_freq());
    if (!lora_state_ok("set frequency", state)) {
        return;
    }

    // set bandwidth to 250 kHz
    state = radio.setBandwidth(ui_lora_get_bandwidth());
    if (!lora_state_ok("set bandwidth", state)) {
        return;
    }

    // set output power to 10 dBm (accepted range is -17 - 22 dBm)
    state = radio.setOutputPower(ui_lora_get_power());
    if (!lora_state_ok("set output power", state)) {
        return;
    }
}
