#include <Arduino.h>

#include "port.h"
#include "config.h"

#include "Hardware/Motors.h"
#include "Hardware/ShiftRegIO.h"

void setup() {
    Serial.begin(SERIAL_BAUD);

    Motors::setup();

}

void loop() {

}
