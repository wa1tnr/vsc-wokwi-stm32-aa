/* nsketch_elvtr-aa.cpp */

/* NUCLEO  L031K6 */

/* elevator 28 March 2025 */

/* Fri 28 Mar 20:37 25 UTC 2025 */

#include <Arduino.h>
#include <stdint.h>

const uint8_t pbswA = 15; // A1 putatively

unsigned long millisAA;
unsigned long millisBB;
const unsigned long duration = 2000;

void reporting() {
  ;
}

bool reading(uint8_t pin) {
  bool r = digitalRead(pin);
  r = !r; // change to positive logic asap
  if (r) {
    delay(300);
  }
  return r;
}

void sayPinMsg(uint8_t pin) {
  Serial.write(' ');
  Serial.write('r');
  Serial.write(' ');
  Serial.print(pin);
}

const uint8_t pbsw[] = {15, 15, 16, 17, 18, 19};

/* const uint8_t led[] = { 1, 1, 2, 3, 4, 5 }; // don't really need a map at all
 */

bool ledNState = 0;

void ledNOnTime() {
  delay(700);
}

void ledNOffTime() {
  delay(1400);
}

void blink(uint8_t ledN) {
  ledNState = !ledNState;
  digitalWrite(ledN, ledNState);
  ledNOnTime();

  ledNState = !ledNState;
  digitalWrite(ledN, ledNState);
  ledNOffTime();
}

void blinkToggle(uint8_t ledNT) {
  digitalToggle(ledNT);
  ledNOnTime();
  digitalToggle(ledNT);
  ledNOffTime();
}

void blinks(uint8_t ledN) {
  pinMode(ledN, OUTPUT);
  for (uint8_t repeat = 3; repeat > 0; repeat--) {
    // blink(ledN);
    blinkToggle(ledN);
  }
}

void ledDemoAA() {
  for (uint8_t ledN = 1; ledN < 6; ledN++) {
    blinks(ledN);
  }
  blinks(7); // PC14 was 23
  blinks(8); // PC15 was 24
  Serial.println(" ledDemoAA() now complete.");
}

void scanPin(uint8_t index) {
  uint8_t pin = pbsw[index];
  bool r = reading(pin);
  if (r) {
    sayPinMsg(pin);
  }
}

void runLoop() {
  for (uint8_t index = 1; index < 6; index++) {
    scanPin(index);
  }
}

void printCR() {
  Serial.println("");
}

void sayPorts() {
  Serial.println("  D15, 16, 17, 18 and 19 for A1..A5");
  Serial.println("  29 March 11:55z:  D23 and D24 for D7 and D8 experiment: ");
}

void initSerial() {
  Serial.begin(9600);
  Serial.println("    program begins.");
  printCR();
  sayPorts();
  printCR();
}

void initOutlierGPIOs() {
  Serial.println("initOutlierGPIOs() now: ");
  pinMode(7, OUTPUT); // PC14 tried 23
  pinMode(8, OUTPUT); // PC15 tried 24
  /* ref. */
  /* https://github.com/stm32duino/Arduino_Core_STM32/blob/main/variants/STM32L0xx/L031K(4-6)T_L041K6T/variant_generic.h */
}

void initGPIO() {
  Serial.println("initGPIO() now: ");
  for (uint8_t index = 1; index < 6; index++) {
    pinMode(pbsw[index], INPUT_PULLUP);
    pinMode(index, OUTPUT); // a bit jank but coincidentally does 'work'
  }
  initOutlierGPIOs();
  ledDemoAA();
}

void setup() {
  delay(7400);
  initSerial();
  initGPIO();
}

void loop() {
  runLoop();
  reporting();
}

/* end. */