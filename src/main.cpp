#include <Arduino.h>
#include <LittleFS.h>

#include "port.h"
#include "config.h"

#include "Hardware/Motors.h"
#include "Hardware/ShiftRegIO.h"

#include "Hardware/Endstops.h"

#include  "Web/WebPanel.h"


void setup() {
    Serial.begin(SERIAL_BAUD);

#ifdef ESP32
    LittleFS.begin(true); // format on fail
#else
    LittleFS.begin();
#endif

    pinMode(BUILTIN_LED, OUTPUT);

    Motors::setup();

    WebPanel::SetupPanel();
}

void loop() {
    Motors::tick();

    WebPanel::PanelTick();

    /*// Log endstops
    for (uint8_t i = 1 ; i <= 5 ; i++) {
        Serial.print(endstops.getEndstop(1, Endstops::POSITIVE).pressing()); Serial.print(" ");
        Serial.print(endstops.getEndstop(1, Endstops::POSITIVE).pressing()); Serial.print("    ");
    }
    Serial.println();*/

    // Log stepper pos
    static unsigned long motor_log_tmr = millis();
    if (millis() - motor_log_tmr >= 500) {
        motor_log_tmr = millis();
        for (auto stepper: Motors::steppers) {
            Serial.print(stepper.pos);
            Serial.print("\t");
        }
        Serial.print("|\t");
        for (auto stepper: Motors::steppers) {
            switch (stepper.getStatus()) {
                case 0:
                    Serial.print("Idle");
                    break;
                case 1:
                    Serial.print("Run");
                    break;
                case 2:
                    Serial.print("Run to P");
                    break;
                case 3:
                    Serial.print("Run by S");
                    break;
                case 4:
                    Serial.print("Stopping");
                    break;
                default:
                    Serial.print("Unknown");
                    break;
            }
            Serial.print("\t");
        }
        Serial.println("");
    }

    static unsigned long blink_tmr = millis();
    static bool blink_state = false;
    if (millis() - blink_tmr > 500) {
        blink_tmr = millis();
        digitalWrite(BUILTIN_LED, blink_state);
        blink_state = !blink_state;
    }
}
