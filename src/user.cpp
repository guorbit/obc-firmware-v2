#include "user.h"
#include <Arduino.h>
#include "config.h"

int initUser() {
  pinMode(GPIO_USER_BUTTON, INPUT); // user button
  return EXIT_SUCCESS;
}

bool checkUser() {
  if (digitalReadFast(GPIO_USER_BUTTON) == HIGH) {
    return true;
  } else {
    return false;
  }
}