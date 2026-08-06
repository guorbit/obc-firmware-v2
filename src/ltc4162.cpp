#include "ltc4162.h"
#include "ina219.h"
#include <math.h>

/***********************************************************************************/
// LTC4162 EPS functions

LTC4162::LTC4162() { i2c_address_ltc = LTC4162_I2C_ADDRESS; }

void LTC4162::begin(TwoWire *theWire, uint8_t addr) {
  _i2c = theWire;
  i2c_address_ltc = addr;
  //_i2c->begin();  // now done in initEPS
  _i2c->beginTransmission(LTC4162_I2C_ADDRESS);
  _i2c->write(LTC4162L_CONFIG_BITS_REG);
  _i2c->endTransmission(false);
  _i2c->requestFrom(LTC4162_I2C_ADDRESS, 2);

  if (_i2c->available() == 2) {
    uint8_t lowByte = _i2c->read();
    uint8_t highByte = _i2c->read();
    uint16_t configVal = (highByte << 8) | lowByte;

    // 2. Set Bit 2 (force_telemetry_on) to 1
    configVal |= (1 << 2);

    // 3. Write it back to the chip
    _i2c->beginTransmission(LTC4162_I2C_ADDRESS);
    _i2c->write(LTC4162L_CONFIG_BITS_REG);
    _i2c->write(configVal & 0xFF);        // Low byte
    _i2c->write((configVal >> 8) & 0xFF); // High byte
    _i2c->endTransmission();

    // Note: The ADC takes ~12ms to warm up after being forced on.
    // Give it a brief delay before trying to read the thermistor.
    delay(20);
  }
}

uint16_t LTC4162::read16(uint8_t reg_addr) {
  _i2c->beginTransmission(i2c_address_ltc);
  _i2c->write(reg_addr);        // register address
  _i2c->endTransmission(false); // end transmission but keep i2c active

  _i2c->requestFrom(i2c_address_ltc, (uint8_t)2); // request 2 bytes
  if (_i2c->available() < 2)
    return 0xFFFF; // error check

  uint16_t lowByte = _i2c->read(); // SMBus uses little-endian (low byte first)
  uint16_t highByte = _i2c->read();

  return (highByte << 8) | lowByte;
}

float LTC4162::readBatteryCurrent() {
  // IBAT: scaling is 1.466µV / RSNSB amperes/LSB
  // Assuming RSNSB = 10mΩ (0.01Ω) as default
  int16_t raw = (int16_t)read16(LTC4162L_IBAT);
  return raw * 1.466e-6f / 0.01f; // µV/LSB / RSNSB in Amps
}

float LTC4162::readBatteryVoltage() {
  // VBAT: scaling is 192.4µV/LSB × cell_count
  // Without cell_count info, using 192.4µV/LSB as baseline
  uint16_t raw = read16(LTC4162L_VBAT);
  return raw * 192.4e-6f * 4; // µV/LSB to Volts
}

uint16_t LTC4162::readBatteryVoltageInt() {
  // VBAT: scaling is 192.4µV/LSB × cell_count
  // Without cell_count info, using 192.4µV/LSB as baseline
  uint16_t raw = read16(LTC4162L_VBAT);
  return raw; // µV/LSB to Volts
}

float LTC4162::readInputVoltage() {
  // VIN: scaling is 1.649mV/LSB
  uint16_t raw = read16(LTC4162L_VIN);
  return raw * 1.649e-3f; // mV/LSB to Volts
}

uint16_t LTC4162::readChargeStatus() { return read16(LTC4162L_CHARGE_STATUS); }

uint16_t LTC4162::readSystemStatus() {
  return read16(LTC4162L_SYSTEM_STATUS_REG);
}

uint16_t LTC4162::readThermistor() {
  return read16(LTC4162L_THERMISTOR_VOLTAGE);
}

float LTC4162::readThermistorProduct() {
  constexpr float Vin = 1.2f;          // [V]
  constexpr float Rt = 10000.0f;       // bias resistor [ohm]
  constexpr float R0 = 10000.0f;       // thermistor resistance at T0 [ohm]
  constexpr float T0 = 298.15f;       // reference temperature [K]
  constexpr float T1 = 273.15f;       // [K] in datasheet 0°C
  constexpr float T2 = 373.15f;       // [K] in datasheet 100°C
  constexpr float RT1 = 28024.0f;     // [ohms] resistance at T1
  constexpr float RT2 = 949.73f;      // [ohms] resistance at T2
  constexpr float maxCount = 21829.0f;

  const float beta = (log(RT1 / RT2)) / ((1.0f / T1) - (1.0f / T2));
  const float Rinf = R0 * exp(-beta / T0);

  const uint16_t thermistorRaw = readThermistor();
  if (thermistorRaw == 0xFFFF || thermistorRaw == 0) {
    return 0.0f;
  }

  const float countRatio = (float)thermistorRaw / maxCount;
  if (countRatio <= 0.0f || countRatio >= 1.0f) {
    return 0.0f;
  }

  const float Vout = Vin * countRatio;
  const float Rout = (Rt * Vout) / (Vin - Vout);
  if (Rout <= 0.0f) {
    return 0.0f;
  }

  const float TempK = (beta / log(Rout / Rinf));
  const float TempC = TempK - 273.15f;

  return isfinite(TempC) ? TempC : 0.0f;
}

uint16_t LTC4162::readFaultStatus() {
  return read16(LTC4162L_CHARGE_STATUS_ALERTS_REG);
}