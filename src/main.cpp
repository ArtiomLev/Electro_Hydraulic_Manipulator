#include <Arduino.h>

#include "port.h"
#include "config.h"

#include "Hardware/Motors.h"

#include "GyverShift.h"
GyverShift<OUTPUT, 1> reg_in(SHIFT_OUT_UPD, SHIFT_OUT_DATA, SHIFT_OUT_CLK);
GyverShift<INPUT, 2> reg_out(SHIFT_IN_PL, SHIFT_IN_DATA, SHIFT_IN_CLK);


void setup() {
    Serial.begin(SERIAL_BAUD);

    Motors::setup();

}

void loop() {

}
