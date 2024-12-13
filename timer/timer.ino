#include <Arduino.h>
#include <TM1637Display.h>

// Module connection pins (Digital Pins)
#define CLK 2
#define DIO 3
#define BUTTON 12

const int buttonPin = 12;

int selectedCountDown = 300;
boolean running = false;
boolean settings = false;
long amountOfMillisecondsPlayed = 0;
long previousMillisecondsForRunning = 0;
long previousMillisecondsForTogglingBrightness = 0;
long previousMillisecondsForButtonPress = 0;
int currentBrightness = 2;
bool buttonPressed = false;
long buttonPressedTime = 0;

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
  updateButtonStateAndTime();

  if (settings) {
    toggleBrightness(500, 1, 3);
    updateDigitsOnTimer(selectedCountDown);

    if (isButtonPressedAndReleasedAfter(1000)) {
      disableSettings();
    } else if (isButtonPressedAndReleasedAfter(0)) {
      changeCountDown();
    }

  } else {

    if (isButtonPressedAndReleasedAfter(4000)) {
      enableSettings();
    } else if (isButtonPressedAndReleasedAfter(1000)) {
      resetTimer();
    } else if (isButtonPressedAndReleasedAfter(0)) {
      pauseOrResumeTimer();
    }

    updateTimer();
  }

  delay(20);
}

void updateButtonStateAndTime() {
  int buttonState = digitalRead(buttonPin);
  if (buttonState == 0 && !buttonPressed) {
    buttonPressedTime = millis();
    buttonPressed = true;
  }
}

bool isButtonPressedAndReleased() {
  int buttonState = digitalRead(buttonPin);
  return buttonPressed && buttonState == 1;
}


bool isButtonPressedAndReleasedAfter(long milliseconds) {
  if (isButtonPressedAndReleased() && millis() - buttonPressedTime >= milliseconds)  {
    buttonPressed = false;
    buttonPressedTime = 0;
    return true;
  }

  return false;
}

void toggleBrightness(long toggleFrequency, int lowBrightness, int highBrightness) {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillisecondsForTogglingBrightness >= toggleFrequency) {
    previousMillisecondsForTogglingBrightness = currentMillis;
    currentBrightness = (currentBrightness == highBrightness) ? lowBrightness : highBrightness;
    display.setBrightness(currentBrightness);
  }
}

void displayDigits(int digits) {
  uint8_t colon = 0b01000000;
  display.showNumberDecEx(digits, colon, true);
}

void displayEnd() {
  toggleBrightness(500, 1, 4);
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
      display.setBrightness(2);
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
