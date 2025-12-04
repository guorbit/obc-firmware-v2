#ifndef HEATER_H
#define HEATER_H

int setHeater(bool state); // declaring heater function

#define GPIO_HEATER PE_4 // defining that the heater is connected to pin PE4.

void heaterInit(); // declaring initialisation

#endif