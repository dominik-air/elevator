#include <Arduino.h>
#include <LinkedList.h>
#include "components.hpp"
#include "elevatorAlgorithm.h"

// ShiftRegister implementation
ShiftRegister::ShiftRegister(int dataPin, int clockPin, int latchPin)
    : dataPin(dataPin), clockPin(clockPin), latchPin(latchPin), currentOutput(0) {
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
}

void ShiftRegister::setPin(int pin, bool state) {
  if (state) {
    currentOutput |= (1 << pin);
  } else {
    currentOutput &= ~(1 << pin);
  }
  write(currentOutput);
}

void ShiftRegister::write(byte value) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, value);
  digitalWrite(latchPin, HIGH);
}

// LED implementation
LED::LED(int pin, ShiftRegister &shiftRegister, unsigned long blinkDuration)
    : shiftRegister(shiftRegister), pin(pin), blinkDuration(blinkDuration), state(OFF), previousMillis(0) {
}

void LED::update() {
  switch (state) {
    case ON:
      updateShiftRegister(true);
      break;
    case OFF:
      updateShiftRegister(false);
      break;
    case BLINKING:
      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis >= blinkDuration) {
        previousMillis = currentMillis;
        updateShiftRegister(!isOn);
      }
      break;
  }
}

void LED::setState(LEDState newState) {
  state = newState;
  update();
}

void LED::updateShiftRegister(bool state) {
  isOn = state;
  shiftRegister.setPin(pin, state);
}

// ButtonHandler implementation
ButtonHandler::ButtonHandler(uint8_t dataPin, uint8_t clockPin, uint8_t latchPin, uint8_t numBits)
  : dataPin(dataPin), clockPin(clockPin), latchPin(latchPin), numBits(numBits) {
  pinMode(dataPin, INPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  buttonStates = new uint8_t[numBits];
  lastButtonStates = new uint8_t[numBits];
  debounceDelay = 50;
  lastDebounceTimes = new unsigned long[numBits];
}

ButtonHandler::~ButtonHandler() {
  delete[] buttonStates;
  delete[] lastButtonStates;
  delete[] lastDebounceTimes;
}

void ButtonHandler::update() {
  digitalWrite(latchPin, LOW);
  digitalWrite(latchPin, HIGH);

  for (int i = numBits - 1; i >= 0; --i) {
    int bit = !digitalRead(dataPin); // Invert the button state (LOW when pressed, HIGH when not pressed)
    if (bit != lastButtonStates[i]) {
      lastDebounceTimes[i] = millis();
    }
    if ((millis() - lastDebounceTimes[i]) > debounceDelay) {
      if (bit != buttonStates[i]) {
        buttonStates[i] = bit;
        lastButtonStates[i] = bit; // Update lastButtonStates after debounce time has passed
      }
    } else {
      lastButtonStates[i] = bit;
    }
    digitalWrite(clockPin, HIGH); // Shift out the next bit
    digitalWrite(clockPin, LOW);
  }
}

bool ButtonHandler::isButtonPressed(uint8_t index) {
  return buttonStates[index] == HIGH;
}

// Elevator implementation
Elevator::Elevator(LED** LEDs, int numLEDs, unsigned long elevatorDelay)
  : LEDs(LEDs), numLEDs(numLEDs), elevatorDelay(elevatorDelay) {
  currentFloor = 0;
  lastElevatorMoveTime = millis();
}

int Elevator::getCurrentFloor() {
  return currentFloor;
}

void Elevator::update(LinkedList<int> &floorsToVisit, LinkedList<int> &elevatorPath) {
  unsigned long currentTime = millis();
  if (currentTime - lastElevatorMoveTime > elevatorDelay) {
    lastElevatorMoveTime = currentTime;
    if (elevatorPath.size() > 0) {

      LEDs[currentFloor]->setState(LED::OFF);
      currentFloor = elevatorPath.shift();

      bool isARequestedFloor = false;
      for (uint8_t i = 0; i < floorsToVisit.size(); ++i) {
        if (currentFloor == floorsToVisit.get(i)) {
          isARequestedFloor = true;
          floorsToVisit.remove(i);
          break;
        }
      }

      if (isARequestedFloor) {
        LEDs[currentFloor]->setState(LED::BLINKING);
      }
      else {
        LEDs[currentFloor]->setState(LED::ON);
      }
    }
    else {
      LEDs[currentFloor]->setState(LED::ON);
    }
  }
}

// SevenSegmentDisplay implementation
// Definicja cyfr 7-segmentowych
const byte digits[] = {
  B00000011, // 0
  B10011111, // 1
  B00100101, // 2
  B00001101, // 3
  B10011001, // 4
  B01001001, // 5
  B01000001, // 6
  B00011111, // 7
  B00000001, // 8
  B00001001  // 9
};

const byte direction[] = {
  B00010011, // góra
  B10000011, // dół
  B11111101, // --
};

SevenSegmentDisplay::SevenSegmentDisplay(int dataPin, int clockPin, int latchPin)
    : dataPin(dataPin), clockPin(clockPin), latchPin(latchPin) {
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  lvl = 0;
}

void SevenSegmentDisplay::Display_state(int currentFloor, LinkedList<int> &elevatorPath){
  if (elevatorPath.size() == 0){
    lvl = 2;
  } else {
    if (elevatorPath.get(0) - currentFloor > 0){
      lvl = 0;
    } else {
      lvl = 1;
    }
  }


  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, digits[currentFloor]);
  shiftOut(dataPin, clockPin, LSBFIRST, direction[lvl]);
  digitalWrite(latchPin, HIGH);
}