#include "eps.h"
#include "ina219.h"
#include "ltc4162.h"

INA219 fiveVolt_monitor;   // create INA219 instance for 5V bus monitoring
INA219 threeVolt3_monitor; // create INA219 instance for 3.3V bus monitoring
LTC4162 bat_ltc;           // create LTC4162 instance for battery management

//*********************************************************************//
//******************** EPS Initialization and Reading ******************//
//*********************************************************************//

void initEPS() {
  // Initialize INA219 monitors
  static TwoWire i2c3(EPS_I2C_SDA_PIN, EPS_I2C_SCL_PIN);
  i2c3.setSDA(EPS_I2C_SDA_PIN);
  i2c3.setSCL(EPS_I2C_SCL_PIN);
  i2c3.begin();

  fiveVolt_monitor.begin(&i2c3, FIVE_VOLT_MONITOR_ADDR);
  fiveVolt_monitor.calibrate();

  threeVolt3_monitor.begin(&i2c3, THREE_VOLT3_MONITOR_ADDR);
  threeVolt3_monitor.calibrate();

  // Initialize LTC4162
  Serial.begin(115200);
  bat_ltc.begin(); // default address 0x67
}

const char *readEPS() {
  static char buffer[64];

  snprintf(buffer, sizeof(buffer), "5V: %.2fV, %.2fW; 3.3V: %.2fV, %.2fW",
           fiveVolt_monitor.busVoltage(),   // 5V bus voltage
           fiveVolt_monitor.busPower(),     // 5V bus power
           threeVolt3_monitor.busVoltage(), // 3.3V bus voltage
           threeVolt3_monitor.busPower());  // 3.3V bus voltage

  /*// read and print EPS values
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
  Serial.print(" V");*/

  return buffer;
}