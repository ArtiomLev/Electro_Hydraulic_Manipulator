#include <Arduino.h>

#include "port.h"
#include "config.h"

#include "GyverShift.h"
GyverShift<OUTPUT, 1> reg_in(SHIFT_OUT_UPD, SHIFT_OUT_DATA, SHIFT_OUT_CLK);
GyverShift<INPUT, 2> reg_out(SHIFT_IN_PL, SHIFT_IN_DATA, SHIFT_IN_CLK);

#include "GyverStepper2.h"
GStepper2<STEPPER2WIRE> steppers[] = {
    GStepper2<STEPPER2WIRE>(MOTOR1_STEP_PER_ROUND * DRV1_MICROSTEPPING, DRV1_STP, DRV1_DIR),
    GStepper2<STEPPER2WIRE>(MOTOR2_STEP_PER_ROUND * DRV2_MICROSTEPPING, DRV2_STP, DRV2_DIR),
    GStepper2<STEPPER2WIRE>(MOTOR3_STEP_PER_ROUND * DRV3_MICROSTEPPING, DRV3_STP, DRV3_DIR),
    GStepper2<STEPPER2WIRE>(MOTOR4_STEP_PER_ROUND * DRV4_MICROSTEPPING, DRV4_STP, DRV4_DIR),
    GStepper2<STEPPER2WIRE>(MOTOR5_STEP_PER_ROUND * DRV5_MICROSTEPPING, DRV5_STP, DRV5_DIR),
};

#include "GyverPlanner2.h"
GPlanner2<STEPPER2WIRE, 5> planner;

void setup() {
    Serial.begin(SERIAL_BAUD);

    // Add steppers to planner
    uint8_t i = 1;
    for (auto stepper: steppers)
        planner.addStepper(i++, stepper);

    // Setup planner
    planner.setMaxSpeed(MAX_PLANNER_SPEED);
    planner.setAcceleration(PLANNER_ACCELERATION);


}

void loop() {

}
