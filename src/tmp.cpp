#include <Arduino.h>

// define temperature pin as PB1, where the onboard TMP36 sensor is connected
#define TMP_PIN PB1

// TODO: measure and set accurate VREF
const float V_REF = 3.3;     // Analog reference voltage (e.g., 5V or 3.3V)
const uint8_t R_BITS = 10;   // ADC resolution (bits)
const float ADC_STEPS = (1 << R_BITS) - 1; // Number of steps (2^R_BITS - 1)

// initialise variables for raw ADC value, derived voltage, and derived temperature
int value = 0;
float voltage = 0.0;
int8_t temp = 0;

int tmp(){
    value = analogRead(TMP_PIN);
    voltage = (value / ADC_STEPS) * V_REF; // convert to voltage
    temp = (voltage - 0.5f) * 100; // convert to temperature in C
    
    return temp;
}