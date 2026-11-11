#ifndef EPS_H
#define EPS_H

#include <Arduino.h>
#include "ina219.h"
#include "ltc4162.h"

constexpr uint8_t FIVE_VOLT_MONITOR_ADDR   = 0x41;   // INA219 address with A0/A1 tied low
constexpr uint8_t THREE_VOLT3_MONITOR_ADDR = 0x40; // INA219 address with A0 tied high, A1 tied low
constexpr uint8_t BATTERY_LTC_ADDR         = 0x68;         // LTC4162 default I2C address

#define EPS_I2C_SDA_PIN PC9
#define EPS_I2C_SCL_PIN PA8

extern INA219 fiveVolt_monitor;
extern INA219 threeVolt3_monitor;
extern LTC4162 bat_ltc;

void initEPS();
const char* readEPS();

#endif // EPS_H