#ifndef BURNWIRE_H
#define BURNWIRE_H

#define GPIO_BURNWIRE PE_3 // defining that the burnwire is connected to pin PE3.

int setBurnwire(bool state); // declaring burnwire function
void initBurnwire(); // declaring initialisation

#endif