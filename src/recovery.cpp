#include <Arduino.h>

char receivedChar;
boolean newData = false;


void recvOneChar() {
    if (Serial.available() > 0) {
        receivedChar = Serial.read();
        newData = true;
    }
}

void showNewData() {
    if (newData) {
        Serial.print("This just in ... ");
        Serial.println(receivedChar);
        newData = false;
    }
}

void recovery()
{
  // this code runs in recovery mode
  Serial.begin(9600);
  Serial.println("Entered recovery mode.");

  while(true) {
    recvOneChar();
    showNewData();
  }
}