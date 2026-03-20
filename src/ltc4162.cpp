#include "ltc4162.h"
#include "ina219.h"

/***********************************************************************************/ 
// LTC4162 EPS functions

LTC4162::LTC4162() {
    i2c_address_ltc = LTC4162_I2C_ADDRESS;
}

void LTC4162::begin(TwoWire *theWire, uint8_t addr) {
    _i2c = theWire;
    i2c_address_ltc = addr;
    //_i2c->begin();  // now done in epsInit
}

uint16_t LTC4162::read16(uint8_t reg_addr) {
    _i2c->beginTransmission(i2c_address_ltc); 
    _i2c->write(reg_addr);                   // register address
    _i2c->endTransmission(false);            // end transmission but keep i2c active  

    _i2c->requestFrom(i2c_address_ltc, (uint8_t)2); // request 2 bytes
    if (_i2c->available() < 2) 
        return 0x0000;         // error check

    uint16_t highByte = _i2c->read(); 
    uint16_t lowByte = _i2c->read(); 
    
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