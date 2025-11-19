#ifndef ADCS_H
#define ADCS_H

// The address of the ADCS board
#define ADCS_I2C_ADDRESS 0x08

// Set the length of the output from the 
//  ADCS board, in bytes.  
#define READOUT_LENGTH_ADCS 32

// Set the I²C clock frequency
#define ADCS_I2C_SPEED 100000

// Set the I²C options
#define ADCS_I2C_SDA PB7
#define ADCS_I2C_SCL PB6

int initADCS();
int readADCS(char* adcsData);
#endif