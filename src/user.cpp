#include "user.h"
#include <Arduino.h>

int initUser() {
  pinMode(PA0, INPUT); // user button
  return EXIT_SUCCESS;
}