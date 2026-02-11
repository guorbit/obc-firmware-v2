#ifndef HEATER_H
#define HEATER_H

#define GPIO_HEATER PE_4 // defining that the heater is connected to pin PE4.

int setHeater(bool state); // declaring heater function
void heaterInit(); // declaring initialisation

#endif