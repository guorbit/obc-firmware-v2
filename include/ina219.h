#ifndef ina219_h
#define ina219_h

#include <Arduino.h>
#include <Wire.h>

#define INA219_DEBUG 0 

// INA219 memory registers
#define INA219_REG_CONFIG		0x00	// configuration register
#define INA219_REG_V_SHUNT		0x01	// differential shunt voltage
#define INA219_REG_VBUS			0x02	// bus voltage (wrt to system/chip GND)
#define INA219_REG_POWER		0x03	// system power draw (= V_BUS * I_SHUNT)
#define INA219_REG_CURRENT		0x04	// shunt current
#define INA219_REG_CALIB		0x05	// calibration register

#define INA219_RESET		    0xFFFF	// send to CONFIG_R to reset unit

#define INA219_CONFIG_DEFAULT	0x399F  // default configuration register value

// config. register bit labels
#define INSA219_BIT_RST	    15      // reset bit
#define INSA219_BIT_BRNG	13      // bus voltage range       
#define INSA219_BIT_PG1	    12      // shunt voltage gain bits
#define INSA219_BIT_PG0	    11
#define INSA219_BIT_BADC4	10      // bus voltage ADC resolution/averaging
#define INSA219_BIT_BADC3	9
#define INSA219_BIT_BADC2	8
#define INSA219_BIT_BADC1	7
#define INSA219_BIT_SADC4	6       // shunt voltage ADC resolution/averaging
#define INSA219_BIT_SADC3	5
#define INSA219_BIT_SADC2	4
#define INSA219_BIT_SADC1	3
#define INSA219_BIT_MODE3	2       // operating mode bits
#define INSA219_BIT_MODE2	1
#define INSA219_BIT_MODE1	0

// default values
#define INA219_DEFAULT_I2C_ADDRESS  0x40    // (64) Default address with A0/A1 tied low
#define INA219_DEFAULT_SHUNT_OHMS   0.1f    // Common 0.1 ohm shunt (was 0.25)
#define INA219_DEFAULT_VBUS_MAX     32.0f   // 32V Bus max (reflecting common 32V range)
#define INA219_DEFAULT_IMAX_EXPECTED 1.0f   // 1 Amp max expected current


class INA219
{
  public:
	INA219();
	
	// by default uses addr = 0x40 (both a-pins tied low)
	void begin(uint8_t addr = INA219_DEFAULT_I2C_ADDRESS);

	void calibrate(float r_shunt  = INA219_DEFAULT_SHUNT_OHMS, 
                   float v_bus_max = INA219_DEFAULT_VBUS_MAX, 
                   float i_max_expected = INA219_DEFAULT_IMAX_EXPECTED);

	void configure(uint16_t config = INA219_CONFIG_DEFAULT);

	void reset();

	int16_t shuntVoltageRaw();
	int16_t busVoltageRaw();
	float shuntVoltage();
	float busVoltage();
	float shuntCurrent();
	float busPower();


  private:
	uint8_t i2c_address;
	float r_shunt, current_lsb, power_lsb;
	uint16_t calibration_value;

	int16_t read16(uint8_t reg_addr);
	void write16(uint8_t reg_addr, uint16_t data);

};

#endif