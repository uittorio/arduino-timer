#include <Arduino.h>
#include <TM1637Display.h>
#include <PinButton.h>

// Module connection pins (Digital Pins)
#define CLK 2
#define DIO 3
#define BUTTON 12

int selectedCountDown = 300;
boolean running = false;
boolean settings = false;
long amountOfMillisecondsPlayed = 0;
long previousMillisecondsForRunning = 0;
long previousMillisecondsForSettings = 0;
int currentBrightness = 2;

PinButton button(BUTTON);

TM1637Display display(CLK, DIO);

void setup()
{
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);

  display.setBrightness(currentBrightness);
  resetTimer();
}

void loop()
{
  button.update();

  if (settings) {
    updateDisplayBrightness();

    if (button.isLongClick()) {
      disableSettings();     
    }

    if (button.isSingleClick()) {
      changeCountDown();
    }
  } else {
    if (button.isLongClick()) {
      enableSettings();
    }

    if (button.isDoubleClick()) {
      resetTimer();
    }

    if (button.isSingleClick()) {
      pauseOrResumeTimer();
    }
  }

  updateTimer();
}

void updateDisplayBrightness() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillisecondsForSettings >= 500) {
    previousMillisecondsForSettings = currentMillis;
    currentBrightness = (currentBrightness == 2) ? 1 : 2;
    display.setBrightness(currentBrightness);
    updateDigitsOnTimer(selectedCountDown);
  }
}

void displayDigits(int digits) {
  uint8_t colon = 0b01000000;
  display.showNumberDecEx(digits, colon, true);
}

void displayEnd() {
  Serial.print("end");
  unsigned char event[4] = {0x71, 0x06, 0x54, 0x79};  
  display.setSegments(event);
}

void pauseOrResumeTimer() {
  if (running) {
    pauseTimer();
  } else {
    resumeTimer();
  }
}

void pauseTimer() {
  running = false;
}

void resumeTimer() {
  running = true;
}

void enableSettings() {
  settings = true;
  resetTimer();
}

void disableSettings() {
  settings = false;
  display.setBrightness(2);
  updateDigitsOnTimer(selectedCountDown);
}

void resetTimer() {
  pauseTimer();
  amountOfMillisecondsPlayed = 0;
  updateDigitsOnTimer(selectedCountDown);
}

void changeCountDown() {
  if (selectedCountDown == 1800) {
    selectedCountDown = 300;
    updateDigitsOnTimer(selectedCountDown);
  } else {
    selectedCountDown = 1800;
    updateDigitsOnTimer(selectedCountDown);
  }
}

void updateTimer() {
  if (running) {
    unsigned long currentMillis = millis();
    amountOfMillisecondsPlayed += currentMillis - previousMillisecondsForRunning;
    previousMillisecondsForRunning = currentMillis;
    unsigned long selectedCountDownInMilliseconds = (long)selectedCountDown * 1000;
    unsigned long millisecondsLefToPlay = selectedCountDownInMilliseconds - amountOfMillisecondsPlayed;

    if (amountOfMillisecondsPlayed > selectedCountDownInMilliseconds) {
      displayEnd();
    } else {
      unsigned long secondsLeftToPlay = millisecondsLefToPlay / 1000;
      updateDigitsOnTimer(ceil(secondsLeftToPlay));  
    }
    
  } else {
    previousMillisecondsForRunning = millis();
  }
}

void updateDigitsOnTimer(int secondsLeft) {
  int minutes = secondsLeft / 60;
  int minutesDividedByTen = minutes / 10;
  int remainingMinutes = minutes % 10;
  int seconds = secondsLeft % 60;
  int secondsDividedByTen = seconds / 10;
  int remainingSeconds = seconds % 10;
  String firstDigit = String(minutesDividedByTen);
  String secondDigit = String(remainingMinutes);
  String thirdDigit = String(secondsDividedByTen);
  String fourthDigit = String(remainingSeconds);
  String allDigit = firstDigit + secondDigit + thirdDigit + fourthDigit;

  int allDigitInt = allDigit.toInt();

  displayDigits(allDigitInt);
}
