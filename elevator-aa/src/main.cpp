/* NUCLEO  L031K6 */
/* elevator 28 March 2025 */

/* Fri 28 Mar 13:05:39 UTC 2025 */

#include <Arduino.h>
#include <stdint.h>

const uint8_t pbswA = 15; // A1 putatively

unsigned long millisAA;
unsigned long millisBB;
const unsigned long duration = 2000;

void reporting() {
    ;
}

bool reading() {
    bool r = digitalRead(pbswA);
    r = ! r; // change to positive logic asap
    if (r) {
        delay(300);
    }
    return r;
}

void runLoop() {
    bool r = reading();
    if (r) {
        Serial.write('r');
        return;
    }
}

void printCR() {
    Serial.println("");
}

void initSerial() {
    delay(3400);
    Serial.begin(9600);
    Serial.println("    program begins.");
    printCR();
}

void initGPIO() {
    pinMode(pbswA, INPUT_PULLUP);
}

void setup() {
    initGPIO();
    initSerial();
}

void loop() {
    runLoop();
    reporting();
}

/* end. */