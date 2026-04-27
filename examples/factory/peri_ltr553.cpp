#include <Wire.h>
#include <SPI.h>
#include <Arduino.h>
#include "SensorLTR553.hpp"
#include "utilities.h"
#include "peripheral.h"

SensorLTR553 als;

bool LTR553_init(void)
{
    if (!als.begin(Wire, LTR553_SLAVE_ADDRESS, BOARD_I2C_SDA, BOARD_I2C_SCL)) {
        Serial.println("Failed to find LTR553 - check your wiring!");
        return false;
    }

    Serial.println("Init LTR553 Sensor success!");

    // Set the ambient light high and low thresholds.
    // If the value exceeds or falls below the set value, an interrupt will be triggered.
    als.setLightSensorThreshold(10, 200);

    // Set the high and low thresholds of the proximity sensor.
    // If the value exceeds or falls below the set value, an interrupt will be triggered.
    als.setProximityThreshold(10, 30);

    // Controls the Light Sensor N number of times the measurement data is outside the range
    // defined by the upper and lower threshold limits before asserting the interrupt.
    als.setLightSensorPersists(5);

    // Controls the Proximity  N number of times the measurement data is outside the range
    // defined by the upper and lower threshold limits before asserting the interrupt.
    als.setProximityPersists(5);

    /*
    *  ALS_IRQ_ACTIVE_LOW, // INT pin is considered active when it is a logic 0 (default)
    *  ALS_IRQ_ACTIVE_HIGH // INT pin is considered active when it is a logic 1
    * * */
    als.setIRQLevel(SensorLTR553::ALS_IRQ_ACTIVE_LOW);

    /*
    *  ALS_IRQ_ONLY_PS,    // Only PS measurement can trigger interrupt
    *  ALS_IRQ_ONLY_ALS,   // Only ALS measurement can trigger interrupt
    *  ALS_IRQ_BOTH,       // Both ALS and PS measurement can trigger interrupt
    * * * */
    als.enableIRQ(SensorLTR553::ALS_IRQ_BOTH);

    /*
    *   ALS_GAIN_1X  ,   -> 1 lux to 64k lux (default)
    *   ALS_GAIN_2X  ,   -> 0.5 lux to 32k lux
    *   ALS_GAIN_4X  ,   -> 0.25 lux to 16k lux
    *   ALS_GAIN_8X  ,   -> 0.125 lux to 8k lux
    *   ALS_GAIN_48X ,   -> 0.02 lux to 1.3k lux
    *   ALS_GAIN_96X ,   -> 0.01 lux to 600 lux
    * */
    als.setLightSensorGain(SensorLTR553::ALS_GAIN_8X);

    /*
    *   PS_LED_PLUSE_30KHZ,
    *   PS_LED_PLUSE_40KHZ,
    *   PS_LED_PLUSE_50KHZ,
    *   PS_LED_PLUSE_60KHZ,
    *   PS_LED_PLUSE_70KHZ,
    *   PS_LED_PLUSE_80KHZ,
    *   PS_LED_PLUSE_90KHZ,
    *   PS_LED_PLUSE_100KHZ,
    * * * * * * * * * * */
    als.setPsLedPulsePeriod(SensorLTR553::PS_LED_PLUSE_100KHZ);

    /*
    *   PS_LED_DUTY_25,
    *   PS_LED_DUTY_50,
    *   PS_LED_DUTY_75,
    *   PS_LED_DUTY_100,
    * * * */
    als.setPsLedDutyCycle(SensorLTR553::PS_LED_DUTY_100);

    /*
    *   PS_LED_CUR_5MA,
    *   PS_LED_CUR_10MA,
    *   PS_LED_CUR_20MA,
    *   PS_LED_CUR_50MA,
    *   PS_LED_CUR_100MA,
    * * * * * * * */
    als.setPsLedCurrent(SensorLTR553::PS_LED_CUR_50MA);

    /*
    *   PS_MEAS_RATE_50MS,
    *   PS_MEAS_RATE_70MS,
    *   PS_MEAS_RATE_100MS,
    *   PS_MEAS_RATE_200MS,
    *   PS_MEAS_RATE_500MS,
    *   PS_MEAS_RATE_1000MS,
    *   PS_MEAS_RATE_2000MS,
    *   PS_MEAS_RATE_10MS
    * * * * * * * */
    als.setProximityRate(SensorLTR553::PS_MEAS_RATE_200MS);

    // Number of pulses
    als.setPsLedPulses(1);

    // Enable proximity sensor saturation indication
    als.enablePsIndicator();

    // Enable ambient light sensor
    als.enableLightSensor();

    // Enable proximity sensor
    als.enableProximity();

    return true;
}

uint16_t LTR_553ALS_get_channel(int ch) // ch 0~1
{
    return als.getLightSensor(ch);
}

uint16_t LTR_553ALS_get_ps(void)
{
    bool saturated = false;
    return als.getProximity(&saturated);
}