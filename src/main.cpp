#include <Arduino.h>
#include "blink.h"
#include "tmp.h"
#include "flash.h"  // SPI flash support 
#include "recovery.h"
#include "watchdog.hpp"
#include "time.h"   // RTC support
#include "heater.h" // heater function support
#include "adcs.h"
#include "save.h"
#include "config.h"

// Temp comms handling in main
#include "LoRa_E32.h"
#include <HardwareSerial.h>
#define COMMS_BROADCAST_CHANNEL 0x04

// Temp comms init
HardwareSerial uart0(PA10, PA9);
LoRa_E32 comms(&uart0, UART_BPS_RATE_9600); // Config without connect AUX and M0 M1

// Initialise variables
char dataFromADCS[READOUT_LENGTH_ADCS] = "ADCS data not gathered\0";
char obcMessage[OBC_MESSAGE_LEN] = {};
unsigned long lastPrint = 0;

void setup() {
    // -------------------- Setup --------------------

    // Debug mode
    #if OBC_DEBUG
    Serial.begin(460800);        // initialize serial for debug output
    #endif

    // Initialise everything
    blinkInit();                 // Initialise blinker on status LED
    flashInit();                 // initialize SPI flash
    rtcInit();                   // initialize RTC
    heaterInit();                // initialize heater function
    adcsInit();                  // initialise ADCS

    pinMode(PB2, INPUT);         // recovery mode pin
    if (digitalRead(PB2) == HIGH) {
        recovery();              // enter recovery mode if pin is high
    }

    // Comms CFG pin
    pinMode(PC6, OUTPUT);
    digitalWriteFast(PC_6, LOW);

    comms.begin();

    iwdg::init_watchdog();
}

void loop() {
    // -------------------- Main Loop --------------------
    // Short delay to slow down loop
    iwdg::pet_watch_dog();
    delay(50);
    iwdg::pet_watch_dog();

    // Debug mode
    #if OBC_DEBUG
    // Print the temperature every loop
    Serial.printf("TMP: %i\n", tmp());
    #endif

    // Blink the status LED to show that the loop is running correctly
    blinkPoll();

    iwdg::pet_watch_dog();

    // Every SLOW_LOOP_FREQ ms
    if (millis() - lastPrint >= SLOW_LOOP_FREQ) {
        // Clear OBC message
        obcMessage[0] = '\0';

        //Serial.printf("RTC Time: %s\n", rtcGetTime());


        iwdg::pet_watch_dog();

        // Collect ADCS data
        adcsRead(dataFromADCS);


        iwdg::pet_watch_dog();

        // Compile message
        snprintf(obcMessage, sizeof(obcMessage), "[%s",
            rtcGetTime()); // begin buffer with > and time
        snprintf(obcMessage + strlen(obcMessage),
            sizeof(obcMessage) - strlen(obcMessage), "|%+02i",
            tmp()); // append TMP value to buffer
        snprintf(obcMessage + strlen(obcMessage),
            sizeof(obcMessage) - strlen(obcMessage), "|%s",
            dataFromADCS); // append ADCS data to buffer


        // End buffer with closing square brace
        snprintf(obcMessage + strlen(obcMessage),
            sizeof(obcMessage) - strlen(obcMessage), "]");


        // Save message
        saveState(obcMessage, strlen(obcMessage));

        // Send message
        iwdg::pet_watch_dog();

        //comms.sendMessage(dataFromADCS);
        //comms.sendBroadcastFixedMessage(COMMS_BROADCAST_CHANNEL, dataFromADCS);


        // [Debug mode] print message
        #if OBC_DEBUG

        Serial.println(obcMessage);


        iwdg::pet_watch_dog();

        #endif

        iwdg::pet_watch_dog();
        
        // Reset lastPrint
        lastPrint = millis();
    }
}
