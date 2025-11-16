// LTC4162 EPS functions

#ifndef ltc4162_h 
#define ltc4162_h 

#include <Arduino.h> 
#include <Wire.h>

#define LTC4162_I2C_ADDRESS 0x68        // Default I2C address 
#define LTC4162_REG_VBAT 0x3A           // Battery Voltage Register
#define LTC4162_REG_IBAT 0x3C           // Battery Current Register 
#define LTC4162_REG_VIN 0x3E            // Input Voltage Register 
#define LTC4162_REG_CHG_STATUS 0x20     // Charge Status Register 
#define LTC4162_REG_SYS_STATUS 0x22     // System Status Register 
#define LTC4162_REG_FAULT 0x1E          // Fault Register 


class LTC4162 {

 public: LTC4162(); 

    void begin(uint8_t addr = LTC4162_I2C_ADDRESS); 
    float readBatteryVoltage(); 
    float readBatteryCurrent(); 
    float readInputVoltage(); 

    uint16_t readChargeStatus(); 
    uint16_t readSystemStatus(); 
    uint16_t readFaultStatus(); 

 private: 
    uint8_t i2c_address_ltc; 
    uint16_t read16(uint8_t reg_addr); 
}; 

#endif