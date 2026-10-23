#include <Arduino.h>

/** 
 * @brief Define temperature pin as PB1, where the onboard TMP36 sensor is connected.
 */
#define TMP_PIN PB1

/**
 * @name ADC Configuration
 * @brief The analog reference voltage is set to 3.3V and ADC resolution bits is set to 10.
 * The number of steps will be (2^R_BITS - 1).
 * @{
 */
const float V_REF = 3.3;                   /*!< Analog reference voltage in Volts */
const uint8_t R_BITS = 10;                 /*!< ADC resolution in bits */
const float ADC_STEPS = (1 << R_BITS) - 1; /*!< Maximum ADC step value */
/** @} */

/** 
 * @name Temperature State Variables
 * @brief Global variables for raw ADC value, derived voltage, and derived temperature.
 * @{
 */
int value = 0;         /*!< Raw ADC value read from the sensor */
float voltage = 0.0;   /*!< Converted voltage output */
int8_t temp = 0;       /*!< Calculated temperature in degrees Celsius */
/** @} */

/**
 * @brief Reads the analog value from the temperature pin and computes the temperature.
 * 
 * @details This function reads the analog value into the global `value` variable. 
 * It is converted to voltage using the ADC and reference voltage. Finally, it is 
 * converted into temperature using the TMP36 conversion formula: (Vout - 0.5) * 100, 
 * where Vout is the voltage output from the sensor.
 * 
 * @return int The temperature in degrees Celsius, or -98 if the reading is out of bounds.
 */
int tmp(){
    value = analogRead(TMP_PIN);
    voltage = (value / ADC_STEPS) * V_REF; // convert to voltage
    temp = (voltage - 0.5f) * 100; // convert to temperature in C
    

    // Never go above or below 99C, with -98 as error value
    if (temp > 99 || temp < -99) {
        temp = -98;
    }

    return temp;
};