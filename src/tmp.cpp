#include <Arduino.h>

/** 
 * define temperature pin as PB1, where the onboard TMP36 sensor is connected
 */
#define TMP_PIN PB1

/***********************************************************************************
 * The Analog reference voltage is set to 3.3V and ADC resolution bits is set to 10.
 * The Number of steps then will be 2^R_BITS - 1).
************************************************************************************/
const float V_REF = 3.3;     
const uint8_t R_BITS = 10;  
const float ADC_STEPS = (1 << R_BITS) - 1; 
/** initialise variables for raw ADC value, derived voltage, and derived temperature  
*/
int value = 0;
float voltage = 0.0;
int8_t temp = 0;

 /*************************************************************************************
  *  The functions reads the analogue value from the tenmperature pin and stores it 
  * into the value variable. It is then converted to voltage using the ADC and reference 
  * voltage. Finally, it is converted into temperature using the TMP36 conversion formula,
  * which is (Vout - 0.5) * 100, where Vout is the voltage output from the sensor.
  * The function returns the temperature as an integer value in degrees Celsius.
  ****************************************************************************************/
int tmp(){
    value = analogRead(TMP_PIN);
    voltage = (value / ADC_STEPS) * V_REF; 
    temp = (voltage - 0.5f) * 100; 
    return temp;
};