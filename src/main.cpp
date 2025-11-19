#include <Arduino.h>
#include "blink.h"
#include "tmp.h"
#include "flash.h"  // SPI flash support
#include "recovery.h"
#include "watchdog.hpp"
#include "time.h"   // RTC support
#include "adcs.h"
#include "save.h"


// Temp comms handling in main
#include "LoRa_E32.h"
#include <HardwareSerial.h>

// Temp comms innit
HardwareSerial uart0(PA9, PA10);
LoRa_E32 comms(&uart0, UART_BPS_RATE_9600); // Config without connect AUX and M0 M1


// Initialise character variables
char dataFromADCS[READOUT_LENGTH_ADCS] = "ADCS data not gathered";

void setup() {
    // -------------------- Setup --------------------
    pinMode(PD13, OUTPUT);       // status LED
    Serial.begin(115200);        // initialize serial for debug output

    flashInit();                 // initialize SPI flash
    rtcInit();                   // initialize RTC
    initADCS();                  // initialise ADCS

    pinMode(PB2, INPUT);         // recovery mode pin
    if (digitalRead(PB2) == HIGH) {
        recovery();              // enter recovery mode if pin is high
    }

    iwdg::init_watchdog();
}

void loop() {
    // -------------------- Main Loop --------------------
    blink(PD_13);                     // blink status LED
    Serial.printf("TMP: %i\n", tmp());  // print TMP value


    iwdg::pet_watch_dog();

    // Optional: print RTC time periodically
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint >= 10000) {
        Serial.printf("RTC Time: %s\n", rtcGetTime());
        lastPrint = millis();
        readADCS(dataFromADCS);

        iwdg::pet_watch_dog();

        Serial.println(dataFromADCS);

        saveState(dataFromADCS, strlen(dataFromADCS));

        iwdg::pet_watch_dog();

        comms.sendMessage(dataFromADCS);

        iwdg::pet_watch_dog();
    }

    delay(50);
    iwdg::pet_watch_dog();
}