#include <Wire.h>
#include <RtcDS3231.h>
#include "DigitLedDisplay.h"

RtcDS3231<TwoWire> Rtc(Wire);

// pin number for tilt switch
const int tiltPin = 3;

// miscellenious variables
int tiltState;

// Variables for time
int startTime;
int timeNow;
int elapsedTime;
int elapsedHours;
int elapsedMinutes;
int elapsedSeconds;

//pin number for the fan
int fanPin = 11;

// Arduino Pin to display Pin for 8X 7-segment display
#define DIN 5
#define CS 6
#define CLK 7
DigitLedDisplay ld = DigitLedDisplay(DIN, CS, CLK);

void setup() {
  pinMode(tiltPin, INPUT);  // Set tilt switch as an input

  pinMode(fanPin, OUTPUT);  // sets the fan as output

  Serial.begin(9600);
  Wire.begin();

  // Brightness min:1, max:15
  ld.setBright(15);

  // Number of digits on the display
  ld.setDigitLimit(8);

  // Computer's date and time is stored in compiled, compiled is the argument
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  Rtc.SetDateTime(compiled);
}

void loop() {

  // Check the state of the tilt-switch
  tiltState = digitalRead(tiltPin);

  //timer function for when the door is locked
  timer();

  //fan for after the door has been unlocked
  startFan();

}

//time of when the tiltswitch was tilted
void timeStart() {
  RtcDateTime now = Rtc.GetDateTime();
  startTime = now.Hour() * 3600 + now.Minute() * 60 + now.Second();
}

// Main function that updates the clock with each passing second since the tiltswitch was tilted.
void timer() {
  // if the tiltswitch is activated the timer will start
  if (tiltState < 1)  {

    timeStart();

    while (tiltState < 1) {

      updateRTC();

      timeElapsed();

      displayPrint();

      // Check the state of the tilt-switch
      tiltState = digitalRead(tiltPin);
    }
    elapsedMinutes = 0;
  }
}

// function for checking the time right now
void updateRTC() {
  RtcDateTime now = Rtc.GetDateTime();
  timeNow = now.Hour() * 3600 + now.Minute() * 60 + now.Second();
}

// help function for timer, and startFan functions that calculates the difference from now since the start of the timer
void timeElapsed() {
  elapsedTime = timeNow - startTime;

  // Converts the elapsed time into hours, minutes, and seconds
  elapsedHours = floor(elapsedTime / 3600);
  elapsedMinutes = floor((elapsedTime % 3600) / 60);
  elapsedSeconds = elapsedTime % 60;
}


// Displays the time on the 7-segment display using the function printDigit from the DigitalLedDisplay library
void displayPrint() {
  ld.printDigit(elapsedHours, 6);
  ld.printDigit(elapsedMinutes, 3);
  ld.printDigit(elapsedSeconds, 0);
  delay(1000);
  ld.clear();
}

// Function for the fan, which starts after the tiltswitch is switched back and then spins for 1 minute or if the tiltswitch is tilted again.
void startFan() {
  if (tiltState > 0) {

    timeStart();

    while (elapsedMinutes < 1 && tiltState > 0) {

      updateRTC();

      timeElapsed();

      digitalWrite(fanPin, HIGH); //starts the fan

      // Check the state of the tilt-switch
      tiltState = digitalRead(tiltPin);
    }
  }

  digitalWrite(fanPin, LOW);  //stops fan

}
