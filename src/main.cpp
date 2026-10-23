// -------------------- Headers --------------------
// General headers
#include "config.h"
#include <Arduino.h>
// Subsystem headers
//#include "blink.h"
#include "adcs.h"
#include "comms.h"
#include "eps.h"
#include "flash.h"  
#include "save.h"
#include "heater.h" 
//#include "burnwire.h"
#include "recovery.h"
#include "time.h" 
#include "tmp.h"
#include "watchdog.hpp"
#include "user.h"

// ------------- Initialize variables --------------
char dataFromADCS[READOUT_LENGTH_ADCS] = {"No data from ADCS"};
char obcMessage[OBC_MESSAGE_LEN] = {};
unsigned long lastPrint = 0;

// --------------------- Setup ---------------------
void setup() {
  delay(8000); // Delay to allow time for connection.

  // Debug mode
#if OBC_DEBUG
  Serial.begin(115200); // initialize serial for debug output
#endif

  // Initialise everything
  //initBlink();    // Initialise blinker on status LED
  initFlash();    // initialize SPI flash
  initRTC();      // initialize RTC
  initHeater();   // initialize heater function
  //initBurnwire(); // initialize burnwire function
  initADCS();     // initialise ADCS
  initEPS();      // initialise EPS
  initComms();    // initialise comms
  initRec();      // initialise recovery mode
  initUser();     // initialise user button

  checkRec();     // check if recovery mode should be entered

  //getComms();  
  setComms();     // configure minimum power and broadcast addressing


  iwdg::init_watchdog();
}

// -------------------- Main Loop --------------------
void loop() {
  // Short delay to slow down loop
  iwdg::pet_watch_dog();
  //delay(50);
  iwdg::pet_watch_dog();

  // User button to set heater and burnwire for testing
  // if (digitalReadFast(PA_0) == HIGH) {
  //   // If pressed, set both to HIGH
  //   digitalWriteFast(PE_4, HIGH);
  //   digitalWriteFast(PE_3, HIGH);
  // } else {
  //   // Else ensure they are LOW
  //   digitalWriteFast(PE_4, LOW);
  //   digitalWriteFast(PE_3, LOW);
  // }

// Debug mode
#if OBC_DEBUG
  // Print the temperature every loop
  //Serial.printf("TMP: %i\n", tmp());
#endif



  // Blink the status LED
  //blinkPoll();

  iwdg::pet_watch_dog();

  // Every SLOW_LOOP_FREQ ms (Message Compilation)
  if (millis() - lastPrint >= SLOW_LOOP_FREQ) {
    // Clear OBC message
    obcMessage[0] = '\0';

    iwdg::pet_watch_dog();

    iwdg::pet_watch_dog();

    // Begin buffer with opening square brace
    snprintf(obcMessage, sizeof(obcMessage), "[");
 
    // Compile message
    iwdg::pet_watch_dog();
    snprintf(obcMessage + strlen(obcMessage),
             sizeof(obcMessage) - strlen(obcMessage), "%s",
             rtcGetTime()); // append time to buffer

    iwdg::pet_watch_dog();
    snprintf(obcMessage + strlen(obcMessage),
             sizeof(obcMessage) - strlen(obcMessage), "|%+02i",
             tmp()); // append TMP value to buffer

    // Collect ADCS data
    adcsRead(dataFromADCS);
    iwdg::pet_watch_dog();
    snprintf(obcMessage + strlen(obcMessage),
             sizeof(obcMessage) - strlen(obcMessage), "|%s",
             dataFromADCS); // append ADCS data to buffer

    iwdg::pet_watch_dog();
    snprintf(obcMessage + strlen(obcMessage),
             sizeof(obcMessage) - strlen(obcMessage), "|%s",
             readEPS()); // append EPS readings to buffer

    // Add heater status
    snprintf(obcMessage + strlen(obcMessage), 
             sizeof(obcMessage) - strlen(obcMessage), "|%d", 
             getHeater());

    // End buffer with closing square brace
    iwdg::pet_watch_dog();
    snprintf(obcMessage + strlen(obcMessage),
             sizeof(obcMessage) - strlen(obcMessage), "]\n");

    // Save message
    iwdg::pet_watch_dog();
    saveState(obcMessage, strlen(obcMessage));


#if OBC_DEBUG
    Serial.println(obcMessage);
#endif

    // Reset lastPrint
    lastPrint = millis();
  }

  iwdg::pet_watch_dog();


  // TEMPERATURE DEPENDENT ACTIONS
  const int boardTemperature = tmp();
  const float batteryTemperature = bat_ltc.readThermistorProduct();

  // Poll sendComms unless a measured temperature indicates overheating
  if (boardTemperature <= 65 &&
      (!isfinite(batteryTemperature) || batteryTemperature <= 65.0f)) {
    sendComms(obcMessage);
  }

  // Turn on heater if OBC is below 10°C and battery is not above 45°C
  if (boardTemperature <= 10 && 
      (!isfinite(batteryTemperature) || batteryTemperature < 45.0f)) {
    setHeater(1);
  } else if (boardTemperature >= 15 || batteryTemperature >= 65.0f) {
    setHeater(0);
  } 
}
