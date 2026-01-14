#include <Arduino.h>

#include "port.h"
#include "config.h"

#include "Hardware/Motors.h"
#include "Hardware/ShiftRegIO.h"

#include "Hardware/Endstops.h"

void setup() {
    Serial.begin(SERIAL_BAUD);

    Motors::setup();

}

void loop() {

    for (uint8_t i = 1 ; i <= 5 ; i++) {
        Serial.print(endstops.getEndstop(1, Endstops::POSITIVE).pressing()); Serial.print(" ");
        Serial.print(endstops.getEndstop(1, Endstops::POSITIVE).pressing()); Serial.print("    ");
    }
    Serial.println();

}
