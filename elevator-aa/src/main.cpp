/* NUCLEO  L031K6 */
/* elevator 28 March 2025 */

/* Fri 28 Mar 17:46 41 UTC 2025 */

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
}

void initSerial() {
    Serial.begin(9600);
    Serial.println("    program begins.");
    printCR();
    sayPorts();
    printCR();
}

void initGPIO() {
    Serial.println("initGPIO() now: ");
    for (uint8_t index = 1; index < 6; index++) {
        pinMode(pbsw[index], INPUT_PULLUP);
        pinMode(index, OUTPUT); // a bit jank but coincidentally does 'work'
    }
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