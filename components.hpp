#include <Arduino.h>
#include <LinkedList.h>

#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

class ShiftRegister {
  public:
    ShiftRegister(int dataPin, int clockPin, int latchPin);
    void setPin(int pin, bool state);
    void write(byte value);

  private:
    int dataPin;
    int clockPin;
    int latchPin;
    byte currentOutput;
};

class LED {
  public:
    LED(int pin, ShiftRegister &shiftRegister, unsigned long blinkDuration);
    enum LEDState { OFF, ON, BLINKING };
    void update();
    void setState(LEDState newState);

  private:
    int pin;
    LEDState state;
    bool isOn;
    unsigned long previousMillis;
    ShiftRegister &shiftRegister;
    unsigned long blinkDuration;

    void updateShiftRegister(bool state);
};

class ButtonHandler {
  public:
    ButtonHandler(uint8_t dataPin, uint8_t clockPin, uint8_t latchPin, uint8_t numBits);
    ~ButtonHandler();

    void update();
    bool isButtonPressed(uint8_t index);

  private:
    uint8_t dataPin, clockPin, latchPin, numBits;
    uint8_t* buttonStates;
    uint8_t* lastButtonStates;
    uint16_t debounceDelay;
    unsigned long* lastDebounceTimes;
};

class Elevator {
  public:
    Elevator(LED** LEDs, int numLEDs, unsigned long elevatorDelay);
    void update(LinkedList<int> &floorsToVisit, LinkedList<int> &elevatorPath);
    int getCurrentFloor();
  
  private:
    int currentFloor;
    int numLEDs;
    LED** LEDs; 
    unsigned long elevatorDelay;
    unsigned long lastElevatorMoveTime;
};

class SevenSegmentDisplay{
  public:
    SevenSegmentDisplay(int dataPin, int clockPin, int latchPin);
    void Display_state(int currentFloor, LinkedList<int> &elevatorPath);

  private:
    int dataPin;
    int clockPin;
    int latchPin;
    int lvl;
};

#endif