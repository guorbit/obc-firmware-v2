#include "eps.h"
#include "ina219.h"
#include <ina219.cpp>

INA219 fiveVolt_monitor; // create INA219 instance for 5V bus monitoring
INA219 threeVolt3_monitor; // create INA219 instance for 3.3V bus monitoring
LTC4162 bat_ltc;          // create LTC4162 instance for battery management

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

//*********************************************************************//
//******************** EPS Initialization and Reading ******************//
//*********************************************************************//

void initEPS() {
    // Initialize INA219 monitors
  fiveVolt_monitor.begin(0x40);     // default address with A0/A1 tied low
  fiveVolt_monitor.calibrate();

  threeVolt3_monitor.begin(0x41);   // address with A0 tied high, A1 tied low
  threeVolt3_monitor.calibrate(); 

    // Initialize LTC4162
  Serial.begin(115200);
  bat_ltc.begin();                  // default address 0x67
}

void readEPS() {                    // read and print EPS values
    Serial.print("5V Bus Voltage: ");
    Serial.println(fiveVolt_monitor.busVoltage());
    Serial.print(" V");

    Serial.print("3.3V Bus Voltage: ");
    Serial.println(threeVolt3_monitor.busVoltage());
    Serial.print(" V");

    Serial.print("Battery Voltage: "); 
    Serial.println(bat_ltc.readBatteryVoltage());
    Serial.print(" V");

    Serial.print("Battery Current: "); 
    Serial.println(bat_ltc.readBatteryCurrent());
    Serial.print(" A");

    Serial.print("Input Voltage: "); 
    Serial.println(bat_ltc.readInputVoltage());
    Serial.print(" V");

    delay(500);
}