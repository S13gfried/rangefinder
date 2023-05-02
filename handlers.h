#include <Arduino.h>

#ifndef BUTTON_TOGGLE_ROTATION
  #define BUTTON_TOGGLE_ROTATION 2
#endif
#ifndef DEBOUNCE_THRESHOLD_MS
  #define DEBOUNCE_THRESHOLD_MS 100
#endif
#ifndef HOLD_THRESHOLD
#define HOLD_THRESHOLD 1000
#endif

volatile unsigned long buttonPressTimestamp = 0;
volatile bool buttonPressed = false;
volatile bool buttonReleased = false;

void setupHandlers();
void buttonLoopRoutine();

void handleButtonPress();
void handleButtonRelease();

void shortPressAction();
void longPressAction();

void setupHandlers() 
{
  pinMode(BUTTON_TOGGLE_ROTATION, INPUT);

  attachInterrupt(digitalPinToInterrupt(BUTTON_TOGGLE_ROTATION), handleButtonPress, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_TOGGLE_ROTATION), handleButtonRelease, RISING);
}

void buttonLoopRoutine() 
{
  if (buttonPressed) 
  {
    buttonPressed = false;

    unsigned long pressDuration = millis() - buttonPressTimestamp;
    if (pressDuration < HOLD_THRESHOLD) {
      shortPressAction();
    }
  }

  if (buttonReleased) 
  {
    buttonReleased = false;

    unsigned long pressDuration = millis() - buttonPressTimestamp;
    if (pressDuration >= HOLD_THRESHOLD) {
      longPressAction();
    }
  }
}

void handleButtonPress() 
{
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();

  // Debounce the button press
  if (interruptTime - lastInterruptTime > DEBOUNCE_THRESHOLD_MS) {
    buttonPressTimestamp = interruptTime;
    buttonPressed = true;
  }

  lastInterruptTime = interruptTime;
}

void handleButtonRelease() 
{
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();

  // Debounce the button release
  if (interruptTime - lastInterruptTime > DEBOUNCE_THRESHOLD_MS) {
    buttonReleased = true;
  }

  lastInterruptTime = interruptTime;
}