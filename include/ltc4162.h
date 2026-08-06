// LTC4162 EPS functions

#ifndef ltc4162_h 
#define ltc4162_h 

#include <Arduino.h> 
#include <Wire.h>

// Registers
#define LTC4162L_EN_LIMIT_ALERTS_REG                0x0D
#define LTC4162L_EN_CHARGER_STATE_ALERTS_REG        0x0E
#define LTC4162L_EN_CHARGE_STATUS_ALERTS_REG        0x0F
#define LTC4162L_CONFIG_BITS_REG                    0x14
#define LTC4162L_IIN_LIMIT_TARGET                   0x15
#define LTC4162L_ARM_SHIP_MODE                      0x19
#define LTC4162L_CHARGE_CURRENT_SETTING             0X1A
#define LTC4162L_VCHARGE_SETTING                    0X1B
#define LTC4162L_C_OVER_X_THRESHOLD                 0x1C
#define LTC4162L_MAX_CV_TIME                        0X1D
#define LTC4162L_MAX_CHARGE_TIME                    0X1E
#define LTC4162L_CHARGER_CONFIG                     0x29
#define LTC4162L_CHARGER_STATE                      0x34
#define LTC4162L_CHARGE_STATUS                      0x35
#define LTC4162L_LIMIT_ALERTS_REG                   0x36
#define LTC4162L_CHARGER_STATE_ALERTS_REG           0x37
#define LTC4162L_CHARGE_STATUS_ALERTS_REG           0x38
#define LTC4162L_SYSTEM_STATUS_REG                  0x39
#define LTC4162L_VBAT                               0x3A
#define LTC4162L_VIN                                0x3B
#define LTC4162L_VOUT                               0x3C
#define LTC4162L_IBAT                               0x3D
#define LTC4162L_IIN                                0x3E
#define LTC4162L_DIE_TEMPERATURE                    0x3F
#define LTC4162L_THERMISTOR_VOLTAGE                 0x40
#define LTC4162L_BSR                                0x41
#define LTC4162L_JEITA_REGION                       0x42
#define LTC4162L_CHEM_CELLS_REG                     0x43
#define LTC4162L_ICHARGE_DAC                        0x44
#define LTC4162L_VCHARGE_DAC                        0x45
#define LTC4162L_IIN_LIMIT_DAC                      0x46
#define LTC4162L_VBAT_FILT                          0x47
#define LTC4162L_INPUT_UNDERVOLTAGE_DAC             0x4B

#define LTC4162_I2C_ADDRESS 0x68        // Default I2C address 


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