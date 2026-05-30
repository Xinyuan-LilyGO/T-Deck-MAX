#include <Arduino.h>
#include <Wire.h>

#include <ExtensionIOXL9555.hpp>
#include <HynTouch.h>
#include <TDeckMaxBoard.h>

ExtensionIOXL9555 xl9555_io;

static void touch_key_logger(uint8_t key_id, bool pressed, void *user_data)
{
    (void)user_data;
    Serial.printf("touch_key[%u]=%s\n", key_id, pressed ? "DOWN" : "UP");
}

static bool init_xl9555_for_touch()
{
    if (!xl9555_io.init(Wire, BOARD_I2C_SDA, BOARD_I2C_SCL, XL9555_SLAVE_ADDRESS0)) {
        return false;
    }

    xl9555_io.pinMode(BOARD_XL9555_07_TOUCH_RST, OUTPUT);
    xl9555_io.digitalWrite(BOARD_XL9555_07_TOUCH_RST, LOW);
    delay(20);
    xl9555_io.digitalWrite(BOARD_XL9555_07_TOUCH_RST, HIGH);
    delay(60);
    return true;
}

void setup()
{
    Serial.begin(115200);
    delay(50);

    Wire.begin(BOARD_I2C_SDA, BOARD_I2C_SCL);
    if (!init_xl9555_for_touch()) {
        Serial.println("Failed to find XL9555 - touch reset may not work");
    }

    hyn_touch_attach_xl9555(&xl9555_io);
    hyn_touch_set_key_callback(touch_key_logger, nullptr);

    if (!hyn_touch_init()) {
        Serial.println("Touch init failed");
        return;
    }

    Serial.println("Touch init OK");
}

void loop()
{
    int16_t x[1] = {0};
    int16_t y[1] = {0};
    uint8_t touched = hyn_touch_get_point(x, y, 1);
    if (touched > 0) {
        Serial.printf("touch_x=%d, touch_y=%d, points=%u\n", x[0], y[0], touched);
    }
    delay(5);
}
