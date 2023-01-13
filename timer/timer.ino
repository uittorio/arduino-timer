#include <Arduino.h>
#include <TM1637Display.h>
#include <PinButton.h>

// TODO
// milliseconds precisions on play/pause
// animation at the end
// bip?

 
// Module connection pins (Digital Pins)
#define CLK 2
#define DIO 3
#define BUTTON 12

const long initialTimeDigits = 3000;
const int initialCountDown = 1800;
int countDown = initialCountDown;
unsigned long startedTime;
unsigned long playedTime;

uint8_t colon = 0b01000000;

boolean running = false;

PinButton button(BUTTON);

TM1637Display display(CLK, DIO);

void setup()
{  
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP); 
  
  // Timer display
  display.setBrightness(2);
  initTimer(); 
  countDown = initialCountDown;
  playedTime = 0;  
  startedTime = 0;
}

void loop()
{   
  button.update();

  if (button.isClick()) {     
     running = !running;
     if (running) {         
      startedTime = millis();       
     } else {      
      playedTime = 0;      
     }
   }

   if (button.isLongClick()) {
      running = false;
      countDown = initialCountDown;           
      playedTime = 0;  
      startedTime = 0;      
      initTimer(); 
   }

   if (running) {          
      const long currentTime = millis() - startedTime;       
       
    if (currentTime - playedTime >= 1000) {      
        countDown--;
          
        if (countDown <= 0) {
          running = false;
          countDown = initialCountDown;           
          playedTime = 0;  
          startedTime = 0;      
          initTimer();
        } else {                
          int minutes = countDown/60;
          int minutesDividedByTen = minutes/10;
          int remainingMinutes = minutes % 10;
          int seconds = countDown % 60;
          int secondsDividedByTen = seconds/10;
          int remainingSeconds = seconds % 10;
          String firstDigit = String(minutesDividedByTen);
          String secondDigit = String(remainingMinutes);
          String thirdDigit = String(secondsDividedByTen);
          String fourthDigit = String(remainingSeconds);
          String allDigit = firstDigit + secondDigit + thirdDigit + fourthDigit;
          
          int allDigitInt = allDigit.toInt();
    
          display.showNumberDecEx(allDigitInt, colon); 
          playedTime += 1000;                 
        }        
    }   
   }
}

void initTimer() { 
  display.showNumberDecEx(initialTimeDigits, colon);
}

void initTimes() {
  countDown = initialCountDown;
  playedTime = 0;  
  startedTime = 0;
}
