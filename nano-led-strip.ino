/*
On most Arduino boards (those with the ATmega168 or ATmega328P), 
this function works on pins 3, 5, 6, 9, 10, and 11
*/

#include <EEPROM.h>

#define     GREENPIN  5
#define     REDPIN    6
#define     BLUEPIN   3
#define     BUTTON_PIN  2

#define     DELAY_TIME_MS   20
#define     TOTAL_MODES     4
#define     MODE_ADDRESS    0

int mode = 1;
byte state = 0;

void setup() {
  Serial.begin(115200);

  Serial.println("starting up");

  // pin 13 is the build-in blue LED
  pinMode(GREENPIN, OUTPUT);
  pinMode(REDPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);

  pinMode(BUTTON_PIN, INPUT);

  // read mode from EEPROM, only 4 possible modes
  mode = EEPROM.read(MODE_ADDRESS);

  Serial.print("setup: mode is ");
  Serial.print(mode);
  Serial.println("");

  if (mode > TOTAL_MODES || mode < 0) {
    // probably the first time ever reading EEPROM
    EEPROM.write(MODE_ADDRESS, 0);
    mode = 0;
  }
  
  analogWrite(BLUEPIN, 255);
  analogWrite(REDPIN, 0);
  analogWrite(GREENPIN, 0);

  Serial.println("attaching interrupt");
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleButton, RISING);
}

void(* resetFunc) (void) = 0;

void handleButton() {
  Serial.println("handling button interrupt");
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200) {
    mode = (mode + 1) % TOTAL_MODES;
    Serial.print("handle: mode is now ");
    Serial.print(mode);
    Serial.println("");
    EEPROM.write(MODE_ADDRESS, mode);
    last_interrupt_time = interrupt_time;
    resetFunc();
  } else {
    last_interrupt_time = interrupt_time;
  }
}

void allRed() {
  analogWrite(REDPIN, 255);
  analogWrite(BLUEPIN, 0);
  analogWrite(GREENPIN, 0);
}


void allBlue() {
  analogWrite(BLUEPIN, 255);
  analogWrite(REDPIN, 0);
  analogWrite(GREENPIN, 0);  
}


void allGreen() {
  analogWrite(GREENPIN, 255);
  analogWrite(REDPIN, 0);
  analogWrite(BLUEPIN, 0);    
}

void color01() {
  int r, g, b;

  allBlue();

  // fade from blue to violet
  for (r = 0; r < 256; r++) {
    analogWrite(REDPIN, r);
    delay(DELAY_TIME_MS);
  }
  // fade from violet to red
  for (b = 255; b > 0; b--) {
    analogWrite(BLUEPIN, b);
    delay(DELAY_TIME_MS);
  }
  // fade from red to yellow
  for (g = 0; g < 256; g++) {
    analogWrite(GREENPIN, g);
    delay(DELAY_TIME_MS); 
  }
  // fade from yellow to green
  for (r = 255; r > 0; r--) {
    analogWrite(REDPIN, r);
    delay(DELAY_TIME_MS);   
  }
  // fade from green to teal
  for (b = 0; b < 256; b++) {
    analogWrite(BLUEPIN, b);
    delay(DELAY_TIME_MS);    
  }
  // fade from teal to blue
  for (g = 255; g > 0; g--) {
    analogWrite(GREENPIN, g);
    delay(DELAY_TIME_MS);    
  }
}

void loop() {
  Serial.print("loop: mode is ");
  Serial.print(mode);
  Serial.println("");
  switch (mode) {

    case 0:
      color01();
      break;

    case 1:
      allRed();
      delay(200);
      break;

    case 2:
      allBlue();
      delay(200);
      break;

    case 3:
      allGreen();
      delay(200);
      break;

    default:
      color01();
      break;
  }    
}