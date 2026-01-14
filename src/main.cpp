#include <Arduino.h>

#include "port.h"
#include "config.h"

#include "Hardware/Motors.h"
#include "Hardware/ShiftRegIO.h"

#include "Hardware/Endstops.h"

void setup() {
    Serial.begin(SERIAL_BAUD);

    pinMode(BUILTIN_LED, OUTPUT);

    Motors::setup();

}

void loop() {

    for (uint8_t i = 1 ; i <= 5 ; i++) {
        Serial.print(endstops.getEndstop(1, Endstops::POSITIVE).pressing()); Serial.print(" ");
        Serial.print(endstops.getEndstop(1, Endstops::POSITIVE).pressing()); Serial.print("    ");
    }
    Serial.println();

    static unsigned long blink_tmr = millis();
    static bool blink_state = false;
    if (millis() - blink_tmr > 500) {
        blink_tmr = millis();
        digitalWrite(BUILTIN_LED, blink_state);
        blink_state = !blink_state;
    }
}
