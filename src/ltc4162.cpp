#include "ltc4162.h"
#include "ina219.h"

/***********************************************************************************/ 
// LTC4162 EPS functions

LTC4162::LTC4162() {
    i2c_address_ltc = LTC4162_I2C_ADDRESS;
}

void LTC4162::begin(uint8_t addr) {
    i2c_address_ltc = addr;
    Wire.begin();  // start i2c
}

uint16_t LTC4162::read16(uint8_t reg_addr) {
    Wire.beginTransmission(i2c_address_ltc); 
    Wire.write(reg_addr);                   // register address
    Wire.endTransmission(false);            // end transmission but keep i2c active  

    Wire.requestFrom(i2c_address_ltc, (uint8_t)2); // request 2 bytes
    if (Wire.available() < 2) 
        return 0xFFFF;         // error check

    uint16_t highByte = Wire.read(); 
    uint16_t lowByte = Wire.read(); 
    
    return (highByte << 8) | lowByte; 
}

float LTC4162::readBatteryCurrent() {
    int16_t raw = (int16_t)read16(LTC4162_REG_IBAT);
    return raw / 1000.0f;           // mA -> A
}

float LTC4162::readBatteryVoltage() {
    uint16_t raw = read16(LTC4162_REG_VBAT); 
    return raw / 1000.0f;           // mv -> V
}

float LTC4162::readInputVoltage() {
    uint16_t raw = read16(LTC4162_REG_VIN); 
    return raw / 1000.0f;           // mv -> V
}

uint16_t LTC4162::readChargeStatus() {
    return read16(LTC4162_REG_CHG_STATUS);
}

uint16_t LTC4162::readSystemStatus() {
    return read16(LTC4162_REG_SYS_STATUS);
}

uint16_t LTC4162::readFaultStatus() {
    return read16(LTC4162_REG_FAULT);
}