#include <Arduino.h>
#include <flash.h> 
#include <time.h>
#include "config.h"
#include "recovery.h"
#include "adcs.h" // for automatic time setting
#include "user.h"

char receivedChar;
bool newData = false; 
int status = EXIT_SUCCESS;
// Initialized string time and year values for the validation function
char year[5];
char month[3];
char day[3];
char hours[3];
char minutes[3];
char seconds[3];

void recvOneChar() {              // receive one character
  if (Serial.available() > 0) {
    receivedChar = Serial.read(); // reads a single character
    newData = true;
     // store the character in the char character 
  }
}

void showInput(const char* input, const char* word){ // if the word input is character or sentance input, then they will be shown. 
  if (strcmp(word,"char")==0){
    Serial.println(receivedChar);
    newData= false;
  }
  else if (strcmp(word,"sentence")==0){
    Serial.println(input);
    newData= false;
   }
}
void instructionTemp(char receivedChar){ // Press 'char' to 'excute the action'
  String action;
  Serial.print("Press ");
  Serial.print(receivedChar);
  Serial.print(" to ");
  switch(receivedChar){
    case 'q':
    action="exit.";
    break;
    case 's':
    action="set time";
    break;
    case 'a':
    action="automatic time sync.";
    break;
    case 'y':
    action="confirm";
    break;
    case 'n':
    action="reject.";
    case 't':
    action="set the time";
    break;
    case 'f':
    action="read the flash.";
    break;
    case 'd':
    action="dump entire flash, or any other key to exit.";
    break;

  }
  Serial.print(action.c_str());
}
void handleInput();
void readline(char*buffer,size_t maxlen){
  while (Serial.available() == 0) {} // Wait for input
  size_t len = Serial.readBytesUntil('\n', buffer, maxlen - 1); // Read input until newline, leaving space for null terminator
  buffer[len] = '\0'; // Null-terminate the string  
  // Clear leftover newline or extra characters
    while (Serial.available()) Serial.read();
}

void exitMode() {                  // exit recovery mode
  instructionTemp('q');
  while(!newData) {                // wait for new input
    recvOneChar();
  }
  newData = false;

  switch(receivedChar){            // process input
    case 'q':
    case 'Q':
      showInput("q","char");
      Serial.println("Exiting...");
      status = EXIT_SUCCESS;
      break;

    default:
      Serial.println("Invalid choice.");
      status = EXIT_FAILURE; 
      break;
  }
}

void templateTime(String time1,String time2,String time3){
  Serial.print("Please enter the ");
  Serial.print(time1.c_str());
  Serial.print(", ");
  Serial.print(time2.c_str());
  Serial.print(" and ");
  Serial.print(time3.c_str());
}


 bool validation(const char*input, const char* type){

  // --- DATE VALIDATION LOGIC ---
  if (strcmp(type, "date") == 0){
    if (strlen(input) != 10 || input[4] != '-' || input[7] != '-'){
      Serial.println("Invalid date format. Please use YYYY-MM-DD format.");
      return false; // Now the message prints before the return
    }
    
    // Extract and assign to global variables
    char year[5];
    year[0] = input[0];
    year[1] = input[1];
    year[2] = input[2];
    year[3] = input[3];
    year[4] = '\0'; // Null terminate the string

    char month[3];
    month[0] = input[5];
    month[1] = input[6];
    month[2] = '\0'; // Null terminate the string

    char day[3];
    day[0] = input[8];
    day[1] = input[9];
    day[2] = '\0'; // Null terminate the string

    //convert to integers for range checking
    int yearInt = atoi(year);
    int monthInt = atoi(month);
    int dayint = atoi(day)  ;
  
    if (monthInt < 1 || monthInt > 12 || dayint < 1 || dayint > 31){
      Serial.println("Invalid date values. Please ensure month is 1-12 and day is 1-31.");
      return false;
    }
    return true;
  }

  // --- TIME VALIDATION LOGIC ---
  if (strcmp(type, "time") == 0){
    if (strlen(input) != 8 || input[2] != ':' || input[5] != ':'){
      Serial.println("Invalid time format. Please use HH:MM:SS format."); 
      return false;
    }
    
    // Extract time in hours,minutes and seconds.
    char hours[3];
    hours[0] = input[0];
    hours[1] = input[1];
    hours[2] = '\0'; // Null terminate the string

    char minutes[3];
    minutes[0] = input[3];
    minutes[1] = input[4];
    minutes[2] = '\0'; // Null terminate the string

    char seconds[3];
    seconds[0] = input[6];
    seconds[1] = input[7];
    seconds[2] = '\0'; // Null terminate the string

    int hoursInt = atoi(hours);
    int minutesInt = atoi(minutes);
    int secondsInt = atoi(seconds);


    if (hoursInt < 0 || hoursInt > 23 || minutesInt < 0 || minutesInt > 59 || secondsInt < 0 || secondsInt > 59) {
      Serial.println("Invalid time values. Please ensure hours are 0-23, minutes and seconds are 0-59.");
      return false;
    }
    return true;
  }
  
  // Return false if the type argument was invalid (shouldn't happen here)
  return false;
}

void timeHourMinSec() {      
  bool dateValid = false;
  while (!dateValid) {templateTime("year", "month", "day");
    while (Serial.available() == 0) {}// wait for input
      char Date_Input[11]; // Buffer to hold the date input (10 chars + null terminator)
      readline(Date_Input, 11); // Read the date input using the readline function
      showInput(Date_Input,"sentence");                                
  dateValid=validation(Date_Input,"date");
  
  if (!dateValid) {
      // If invalid, the validation function prints the error message.
      // We loop back to prompt the user again.
      handleInput();
      continue;
    }
  }
  // now get time
  bool timeValid = false;
  while (!timeValid) {
    templateTime("Hours", "Minutes", "Seconds"); // Prompt for time (removed colons)
    while (Serial.available() == 0) {} // Wait for input
    char Time_Input[11]; // Buffer to hold the date input (10 chars + null terminator)
    readline(Time_Input, 11); // Read the date input using the readline function
    showInput(Time_Input,"sentence");      
    // Validate the time, and update timeValid flag
    timeValid = validation(Time_Input, "time");
    if (!timeValid) {
      handleInput();
      continue;
  }                              
  // print formatted time on one line
  Serial.print("Time set to: ");
  char time_display[25];
  sprintf(time_display, "%s-%s-%sT%s:%s:%sZ", year, month, day, hours, minutes, seconds);
  Serial.println(time_display);
}
}

int timeSetManual() {                // manual time setting
  Serial.println("Manual time setting selected.");
  timeHourMinSec();
  status = EXIT_SUCCESS;
  return status;
}

int timeSetAuto() {               // automatic time setting
  Serial.println("Retrieving time from GPS...");

  // Create struct for time and initialise with defaults
  struct {
    int // all of type int
    year = 2026, 
    month = 3, 
    day = 20, 
    hour = 0, 
    minute = 0, 
    second = 0;
  } gpsTime;

  // Get gps time
  char dataFromADCS[READOUT_LENGTH_ADCS] = "ADCS data not gathered\0";
  initADCS();                  // initialise ADCS
  adcsRead(dataFromADCS);

  if (dataFromADCS[0] == 't') {
    // Extract time from adcs message
    sscanf(dataFromADCS, "t%02d%02d%02d", gpsTime.hour, gpsTime.minute, gpsTime.second);
  } else {
    // Print an error
    Serial.printf("No valid time in message: %s\n", dataFromADCS);
  }

  char gpsTimeChar[32] = {};
  snprintf(gpsTimeChar, sizeof(gpsTimeChar), "%04d%02d%02dT%02d%02d%02dZ", 
          gpsTime.year, gpsTime.month, gpsTime.day, 
          gpsTime.hour, gpsTime.minute, gpsTime.second);


  const char* oldTime = rtcGetTime(); 
  Serial.print("Old rtc time: ");
  Serial.println(oldTime);

  // Set rtc time
  rtcSetTime(gpsTimeChar);

  const char* newTime = rtcGetTime(); 
  Serial.print("New rtc time: ");
  Serial.println(oldTime);

  Serial.println("Time synchronized. Please confirm this time is accurate.");
  Serial.println("Press y to confirm, n to reject. ");

      while(!newData) {           // wait for new input
      recvOneChar();
      }
      newData = false;

      switch (receivedChar) {
      case 'y':
      case 'Y':
        showInput("y", "char");
        Serial.println("Time confirmed. Exiting...");
        status = EXIT_FAILURE;
        break;

      case 'n':
      case 'N':
        showInput("n", "char");
        Serial.println("Time rejected. Please enter manual time setting mode.");
        status = timeSetManual();
        break;
      
      default:
        Serial.println("Invalid choice.");
        status = EXIT_FAILURE;
        break;
      }
  return status;
}

int setTimeMode() {                 // stub for setting time
  instructionTemp('s');
  Serial.print(" or ");
  instructionTemp('a');
  while(!newData) {                 // wait for new input
    recvOneChar();
  }
  newData = false;

  switch(receivedChar){
    case 's':
    case 'S':
      showInput("s", "char");
      timeSetManual();                 //manual time set
      status = EXIT_SUCCESS;
      break;

    case 'a':
    case 'A':
      showInput("a", "char");
      timeSetAuto();                //automatic time set
      status = EXIT_SUCCESS;
      break;

    default:
      Serial.println("Invalid choice.");
      status = EXIT_FAILURE;
      break;
  }
  exitMode();
  return status;
}

int flashDumpMode() {               // dump entire flash
  Serial.println('d');

  while(!newData) {                 // wait for new input
    recvOneChar();
  }

  switch(receivedChar){
    case 'd':
    case 'D':
      showInput("d", "char");
      Serial.println("Dumping entire flash...");
      flashDumpAll();
      status = EXIT_SUCCESS;
      break;

    default:
      Serial.println("Exiting...");
      status = EXIT_FAILURE;
      break;
  }
  return status;
}

int flashEraseMode() {
  Serial.println("Press y to erase entire flash, any other key to exit.");
  Serial.println("You can only erase the flash if you unclick the recovery mode button.");

  while(!newData) {                 // wait for new input
    recvOneChar();
  }

  switch(receivedChar){
    case 'y':
    case 'Y':
      if (checkUser()) {
        break;
      }
      Serial.println("Erasing entire flash...");
      flashEraseAll();
      status = EXIT_SUCCESS;
      break;

    default:
      Serial.println("Exiting...");
      status = EXIT_FAILURE;
      break;
  }
  return status;
}

void handleInput() {
  if (newData) {
    newData = false;

    switch (receivedChar){
    case 't':
    case 'T':
      showInput("t", "char");
      status = setTimeMode();
      Serial.println(status);
      break;

    case 'f':
    case 'F':
      showInput("f", "char");
      status = flashDumpMode();
      Serial.println(status);
      break;

    case 'e':
    case 'E': 
      status = flashEraseMode();
      Serial.println(status);
      break;


    default:
      Serial.println("Invalid choice.");
      break;
    }
  }
}

void recovery() {                 // recovery mode
  Serial.begin(460800);              // initialize serial for recovery mode

  Serial.setTimeout(60000);   // set timeout to 60 seconds

  while(!newData) {              // wait for new input 
    recvOneChar();
  }
  newData = false;

  Serial.println("Entered recovery mode. \nPress enter to continue.");
  delay(1000);

  while(!newData) {              // wait for new input 
    recvOneChar();
    
  }
  newData = false;

  while (true) {                // main recovery loop  
    Serial.println("Press 't' to set the time, 'f' to read the flash, or 'e' to erase the flash.");
    newData = false;

    while(!newData) { 
      recvOneChar();
    }
    handleInput(); 

  }
}

int initRec() {              // initialise recovery mode
  pinMode(PB2, INPUT); // recovery mode pin
  return EXIT_SUCCESS;
}

int checkRec() {             // check if recovery mode should be entered
  if (digitalReadFast(PB_2) == HIGH) {
    recovery();
  } 
  return EXIT_SUCCESS;
}
