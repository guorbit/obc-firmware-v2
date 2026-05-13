#ifndef COMMS_H
#define COMMS_H

#define COMMS_BROADCAST_CHANNEL 0x04

int sendComms(const char* state);
int initComms();

#endif