#include <Arduino.h>
#include <flash.h> 
#include <time.h>
#include "recovery.h"
char receivedChar;
bool newData = false; 
int status = EXIT_SUCCESS;
// Initialized string time and year values for the validation function
String year="0";
String month = "0";
String day = "0";
String hours = "0";
String minutes = "0";
String seconds = "0";

void recvOneChar() {              // receive one character
  if (Serial.available() > 0) {
    receivedChar = Serial.read(); // reads a single character
    newData = true;
     // store the character in the char character 
  }
}

void showInput(const String character, const String word){ // if the word input is character or sentance input, then they will be shown. 
  if (word=="char"){
    String character = "";
    character=receivedChar;
    Serial.println(receivedChar);
    newData= false;
  }
  else if (word=="sentence"){
    Serial.println(character);
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
    action="confirm.";
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


 bool validation(const String Input, const String type){

  // --- DATE VALIDATION LOGIC ---
  if (type == "date"){
    if (Input.length() != 10 || Input.charAt(4) != '-' || Input.charAt(7) != '-'){
      Serial.println("Invalid date format. Please use YYYY-MM-DD format.");
      return false; // Now the message prints before the return
    }
    
    // Extract and assign to global variables
    year = Input.substring(0, 4);
    month =Input.substring(5, 7);
    day = Input.substring(8, 10);

    //convert to integers for range checking
    int yearInt = year.toInt();
    int monthInt = month.toInt();
    int dayint = day.toInt();
  
    if (monthInt < 1 || monthInt > 12 || dayint < 1 || dayint > 31){
      Serial.println("Invalid date values. Please ensure month is 1-12 and day is 1-31.");
      return false;
    }
    return true;
  }

  // --- TIME VALIDATION LOGIC ---
  if (type == "time"){
    if (Input.length() != 8 || Input.charAt(2) != ':' || Input.charAt(5) != ':'){
      Serial.println("Invalid time format. Please use HH:MM:SS format."); 
      return false;
    }
    
    // Extract time in hours,minutes and seconds.
    hours = Input.substring(0, 2); 
    minutes = Input.substring(3, 5);
    seconds = Input.substring(6, 8);
    int hoursInt = hours.toInt();
    int minutesInt = minutes.toInt();
    int secondsInt = seconds.toInt();


    if (hoursInt < 1 || hoursInt > 23 || minutesInt < 0 || minutesInt > 59 || secondsInt < 0 || secondsInt > 59) {
      Serial.println("Invalid time values. Please ensure hours are 1-23, minutes and seconds are 0-59.");
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
      String dateInput = Serial.readStringUntil('\n');    //read input until enter
      dateInput.trim(); //remove any leading/trailing whitespace
      showInput(dateInput,"sentence");                                
  dateValid=validation(dateInput,"date");
  
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
    String timeInput = Serial.readStringUntil('\n'); 
    timeInput.trim();
    showInput(timeInput, "sentence"); 
    // Validate the time, and update timeValid flag
    timeValid = validation(timeInput, "time");
    if (!timeValid) {
      handleInput();
      continue;
  }                              
  // print formatted time on one line
  Serial.print("Time set to: ");
  String time_display= year + "-"+ month + "-"+day + "T" + hours + ":"+ minutes + ":"+ seconds + "Z";
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
  const char* currentTime = rtcGetTime(); 
  Serial.print("Current RTC Time: ");
  Serial.print(currentTime);

  Serial.println("Time synchronized. Please confirm this time is accurate.");
  instructionTemp('y');
  Serial.print(" or ");
  instructionTemp('n');

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

    default:
      Serial.println("Invalid choice.");
      break;
    }
  }
}

void recovery() {                 // recovery mode
    // set timeout to 60 seconds
    Serial.println("Entered recovery mode. \nPress any key to continue.");
    Serial.setTimeout(60000);
  while(!newData) {              // wait for new input 
    recvOneChar();
    
  }
  newData = false;

  while (true) {                // main recovery loop  
    instructionTemp('t');
    Serial.print(" or ");
    instructionTemp('f');
    newData = false;

    while(!newData) { 
      recvOneChar();
    }
    handleInput(); 

  }
}