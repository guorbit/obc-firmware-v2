/******************************************************************************
*
* https://github.com/johngineer/ArduinoINA219/blob/master/INA219.cpp
*
* 6 May 2012 by John De Cristofaro, updated November 2025 by Islay Ross for GU Orbit use.
*
* Tested at standard i2c 100kbps signaling rate.
*
* This library does not handle triggered conversion modes. It uses the INA219
* in continuous conversion mode. All reads are from continous conversions.
*
* Known bugs:
*     * may return unreliable values if not connected to a bus or at
*	bus currents below 10uA.
*
* Arduino 1.0 compatible as of 6/6/2012
*
* Dependencies:
*    * Arduino Wire library
*
* MIT license
******************************************************************************/

#include "eps.h"

INA219 fiveVolt_monitor; // create INA219 instance for 5V bus monitoring
INA219 threeVolt3_monitor; // create INA219 instance for 3.3V bus monitoring
LTC4162 bat_ltc;          // create LTC4162 instance for battery management

// Constructor
INA219::INA219() {          // LSBs set to zero until calibration
    r_shunt = 0.0f;
    current_lsb = 0.0f;
    power_lsb = 0.0f;
    calibration_value = 0;
}

void INA219::begin(uint8_t addr)
{
  i2c_address = addr; 
  Wire.begin();  // start i2c
}

// calibration of equations and device
// shunt_val 		= value of shunt in Ohms
// v_shunt_max 		= maximum value of voltage across shunt
// v_bus_max 		= maximum voltage of bus
// i_max_expected 	= maximum current draw of bus + shunt
// default values are for a 0.25 Ohm shunt on a 5V bus with max current of 1A
void INA219::calibrate(float shunt_val, float v_bus_max, float i_max_expected)
{
  r_shunt = shunt_val;

  current_lsb = i_max_expected / 32767.0f;  // minimum LSB based on max expected current
  float cal_float = (0.04096f) / (current_lsb * r_shunt);

  // ensure current_lsb is in a reasonable range
  if (cal_float > 32767.0f) {
    calibration_value = 3276.0f; 
  }
  else {
        calibration_value = (uint16_t)cal_float;
  }
    
  power_lsb = current_lsb * 20.0f; // power LSB is 20 times current LSB
 
#if (INA219_DEBUG == 1)
  Serial.print("i_max_expected: "); Serial.println(i_max_expected, 8);
  Serial.print("current_lsb:	"); Serial.println(current_lsb, 12);
  Serial.print("power_lsb:	"); Serial.println(power_lsb, 8);
  Serial.println("  ");
  Serial.print("cal:		"); Serial.println(cal);
  Serial.print("r_shunt:	"); Serial.println(r_shunt);
#endif

  write16(INA219_REG_CALIB, calibration_value);
  configure(INA219_CONFIG_DEFAULT);
}


// config values (range, gain, bus adc, shunt adc, mode) can be derived from pp26-27 in the datasheet
// defaults are:
// range = 1 (0-32V bus voltage range)
// gain = 3 (1/8 gain - 320mV range)
// bus adc = 3 (12-bit, single sample, 532uS conversion time)
// shunt adc = 3 (12-bit, single sample, 532uS conversion time)
// mode = 7 (continuous conversion)
void INA219::configure(uint16_t config)
{
  write16(INA219_REG_CONFIG, config);		
}

// resets the INA219
void INA219::reset()
{
  write16(INA219_REG_CONFIG, INA219_RESET);
  delay(5);
}

// returns the raw binary value of the shunt voltage
int16_t INA219::shuntVoltageRaw()
{
  return read16(INA219_REG_V_SHUNT);
}

float INA219::shuntVoltage()
{
  float temp;
  temp = read16(INA219_REG_V_SHUNT);
  return (temp / 100000);
}

// returns raw bus voltage binary value
int16_t INA219::busVoltageRaw()
{
  return read16(INA219_REG_VBUS);
}

float INA219::busVoltage()
{
  int16_t temp = read16(INA219_REG_VBUS);
  temp >>= 3;                               // shift right by 3 bits
  return (float)temp * 0.004f;              // LSB = 4mV
}

float INA219::shuntCurrent()
{
  return (read16(INA219_REG_CURRENT) * current_lsb);    // in amps
}

float INA219::busPower()
{
  return (read16(INA219_REG_POWER) * power_lsb);        // in watts
}

// writes a 16-bit word (d) to register pointer (a)
// when selecting a register pointer to read from, (d) = 0
void INA219::write16(uint8_t a, uint16_t d) {
  uint8_t temp;
  temp = (uint8_t)d;
  d >>= 8;
  Wire.beginTransmission(i2c_address); // start transmission to device

  #if ARDUINO >= 100
    Wire.write(a);                      // sends register address to read from
    Wire.write((uint8_t)d);             // write data hibyte 
    Wire.write(temp);                   // write data lobyte;
  #else
    Wire.send(a);                       // sends register address to read from
    Wire.send((uint8_t)d);              // write data hibyte 
    Wire.send(temp);                    // write data lobyte;
  #endif

  Wire.endTransmission();               // end transmission
  delay(1);
}

int16_t INA219::read16(uint8_t a) {
  uint16_t ret;

  // move the pointer to reg. of interest, null argument
  write16(a, 0);
  
  Wire.requestFrom((int)i2c_address, 2);	// request 2 data bytes

  #if ARDUINO >= 100
    ret = Wire.read(); // rx hi byte
    ret <<= 8;
    ret |= Wire.read(); // rx lo byte
  #else
    ret = Wire.receive(); // rx hi byte
    ret <<= 8;
    ret |= Wire.receive(); // rx lo byte
  #endif

  Wire.endTransmission(); // end transmission

  return ret;
}

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


// EPS initialization and reading functions
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