#ifndef EPS_H
#define EPS_H

#include <Arduino.h>
#include "ina219.h"
#include "ltc4162.h"

#define FIVE_VOLT_MONITOR_ADDR 0x40   // INA219 address with A0/A1 tied low
#define THREE_VOLT3_MONITOR_ADDR 0x41 // INA219 address with A0 tied high, A1 tied low
#define BATTERY_LTC_ADDR 0x67         // LTC4162 default I2C address

#define EPS_I2C_SDA_PIN PA8
#define EPS_I2C_SCL_PIN PC9

extern INA219 fiveVolt_monitor;
extern INA219 threeVolt3_monitor;
extern LTC4162 bat_ltc;

void initEPS();
const char* readEPS();

#endif // EPS_H