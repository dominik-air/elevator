#include <Arduino.h>
#include <LinkedList.h>
#include "components.hpp"
#include "elevatorAlgorithm.h"

// Pin assignments
const int outputDataPin = 2;
const int outputClockPin = 3;
const int outputLatchPin = 4;
const int inputDataPin = 5;
const int inputClockPin = 6;
const int inputLatchPin = 7;
const int segLatchPin = A1;
const int segDataPin = A0;
const int segClockPin = A2;

// Number of buttons and LEDs
const int numButtons = 8;
const int numLEDs = 8;

// Button debounce delay (in milliseconds)
const int debounceDelay = 50;

// Declare ShiftRegister for the output (LEDs)
ShiftRegister outputShiftRegister(outputDataPin, outputClockPin, outputLatchPin);

// Declare SevenSegmentDisplay for the output
SevenSegmentDisplay outputSevenSegmentDisplay(segDataPin, segClockPin, segLatchPin);

// Declare LEDs
LED *LEDs[numLEDs];

ButtonHandler buttonHandler(inputDataPin, inputClockPin, inputLatchPin, numButtons);

// Declare requested floors array and path array
LinkedList<int> floorsToVisit;
LinkedList<int> elevatorPath;

Elevator elevator(LEDs, numLEDs, 2000);

void setup() {
  Serial.begin(115200);
  for (uint8_t i = 0; i < numLEDs; i++) {
      LEDs[i] = new LED(i, outputShiftRegister, 500);
      LEDs[i]->setState(LED::OFF);
  }
}

void loop() {
  bool requestedFloors = false;
  for (uint8_t i = 0; i < numButtons; i++) {
    if (buttonHandler.isButtonPressed(i)) {
      bool notFound = true;
      for (uint8_t j = 0; j < floorsToVisit.size(); ++j) {
        if (floorsToVisit.get(j) == i) {
          notFound = false;
          break;
        }
      }
      if (notFound) {
        Serial.print("Added ");
        Serial.print(i);
        Serial.println();
        floorsToVisit.add(i);
        requestedFloors = true;
      }
    }
  }

  if (requestedFloors) {
    elevatorAlgorithm(elevator.getCurrentFloor(), floorsToVisit, elevatorPath);
  }

  elevator.update(floorsToVisit, elevatorPath);
  
  buttonHandler.update();

  for (uint8_t i = 0; i < numLEDs; i++) {
    LEDs[i]->update();
  }

  outputSevenSegmentDisplay.Display_state(elevator.getCurrentFloor(), elevatorPath);
}