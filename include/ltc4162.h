// LTC4162 EPS functions

#ifndef LTC4162_H
#define LTC4162_H 

#include <Arduino.h> 
#include <Wire.h>
#include <cstdint>

// Registers           
constexpr uint8_t LTC4162L_EN_LIMIT_ALERTS_REG         = 0x0D;
constexpr uint8_t LTC4162L_EN_CHARGER_STATE_ALERTS_REG = 0x0E;
constexpr uint8_t LTC4162L_EN_CHARGE_STATUS_ALERTS_REG = 0x0F;
constexpr uint8_t LTC4162L_CONFIG_BITS_REG             = 0x14;
constexpr uint8_t LTC4162L_IIN_LIMIT_TARGET            = 0x15;
constexpr uint8_t LTC4162L_ARM_SHIP_MODE               = 0x19;
constexpr uint8_t LTC4162L_CHARGE_CURRENT_SETTING      = 0X1A;
constexpr uint8_t LTC4162L_VCHARGE_SETTING             = 0X1B;
constexpr uint8_t LTC4162L_C_OVER_X_THRESHOLD          = 0x1C;
constexpr uint8_t LTC4162L_MAX_CV_TIME                 = 0X1D;
constexpr uint8_t LTC4162L_MAX_CHARGE_TIME             = 0X1E;
constexpr uint8_t LTC4162L_CHARGER_CONFIG              = 0x29;
constexpr uint8_t LTC4162L_CHARGER_STATE               = 0x34;
constexpr uint8_t LTC4162L_CHARGE_STATUS               = 0x35;
constexpr uint8_t LTC4162L_LIMIT_ALERTS_REG            = 0x36;
constexpr uint8_t LTC4162L_CHARGER_STATE_ALERTS_REG    = 0x37;
constexpr uint8_t LTC4162L_CHARGE_STATUS_ALERTS_REG    = 0x38;
constexpr uint8_t LTC4162L_SYSTEM_STATUS_REG           = 0x39;
constexpr uint8_t LTC4162L_VBAT                        = 0x3A;
constexpr uint8_t LTC4162L_VIN                         = 0x3B;
constexpr uint8_t LTC4162L_VOUT                        = 0x3C;
constexpr uint8_t LTC4162L_IBAT                        = 0x3D;
constexpr uint8_t LTC4162L_IIN                         = 0x3E;
constexpr uint8_t LTC4162L_DIE_TEMPERATURE             = 0x3F;
constexpr uint8_t LTC4162L_THERMISTOR_VOLTAGE          = 0x40;
constexpr uint8_t LTC4162L_BSR                         = 0x41;
constexpr uint8_t LTC4162L_JEITA_REGION                = 0x42;
constexpr uint8_t LTC4162L_CHEM_CELLS_REG              = 0x43;
constexpr uint8_t LTC4162L_ICHARGE_DAC                 = 0x44;
constexpr uint8_t LTC4162L_VCHARGE_DAC                 = 0x45;
constexpr uint8_t LTC4162L_IIN_LIMIT_DAC               = 0x46;
constexpr uint8_t LTC4162L_VBAT_FILT                   = 0x47;
constexpr uint8_t LTC4162L_INPUT_UNDERVOLTAGE_DAC      = 0x4B;

constexpr uint8_t LTC4162_I2C_ADDRESS = 0x68;        // Default I2C address 


class LTC4162 {

 public: LTC4162(); 

    void begin(TwoWire *theWire, uint8_t addr = LTC4162_I2C_ADDRESS); 
    float readBatteryVoltage(); 
    float readBatteryCurrent(); 
    float readInputVoltage(); 
    float readThermistorProduct();

    uint16_t readBatteryVoltageInt();

    uint16_t readThermistor();
    uint16_t readChargeStatus(); 
    uint16_t readSystemStatus(); 
    uint16_t readFaultStatus(); 

 private: 	
    TwoWire *_i2c;
    uint8_t i2c_address_ltc; 
    uint16_t read16(uint8_t reg_addr); 
}; 

#endif