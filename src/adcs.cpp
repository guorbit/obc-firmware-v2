#include "adcs.h"
#include <Wire.h>
#include <Arduino.h>

TwoWire i2c0 = TwoWire(0);

int initADCS() {
    i2c0.begin(ADCS_I2C_SDA, ADCS_I2C_SCL, 100000);
    return EXIT_SUCCESS;
}

int readADCS(char* adcsData) { 
    i2c0.requestFrom(ADCS_I2C_ADDRESS, READOUT_LENGTH_ADCS);

    for (int i = 0; i < READOUT_LENGTH_ADCS; i++) {
        adcsData[i] = i2c0.read();
    }

    /*
    while (i2c0.available()) {
        char c = i2c0.read();
        Serial.println(c);
    }
    */

    return EXIT_SUCCESS;
}
