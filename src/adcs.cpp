#include "adcs.h"
#include <Wire.h>
#include <Arduino.h>

TwoWire i2c1(ADCS_I2C_SDA, ADCS_I2C_SCL);

int initADCS() {
    i2c1.setSDA(ADCS_I2C_SDA);
    i2c1.setSCL(ADCS_I2C_SCL);
    i2c1.begin(); //, ADCS_I2C_SPEED);
    return EXIT_SUCCESS;
}

int readADCS(char* adcsData) { 
    int bytesRecieved = i2c1.requestFrom(ADCS_I2C_ADDRESS, READOUT_LENGTH_ADCS);

    for (int i = 0; i < READOUT_LENGTH_ADCS; i++) {
        if (i2c1.available()) {
            adcsData[i] = i2c1.read();
        } 
    }

    /*
    while (i2c1.available()) {
        char c = i2c1.read();
        Serial.println(c);
    }
    */

    return EXIT_SUCCESS;
}
