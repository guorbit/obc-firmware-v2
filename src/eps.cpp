#include "ltc4162.h"
#include "ina219.h"

INA219 fiveVolt_monitor; // create INA219 instance for 5V bus monitoring
INA219 threeVolt3_monitor; // create INA219 instance for 3.3V bus monitoring
LTC4162 bat_ltc;          // create LTC4162 instance for battery management

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