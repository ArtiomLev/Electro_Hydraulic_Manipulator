#include <Arduino.h>

#include "port.h"
#include "config.h"

#include "GyverShift.h"
GyverShift<OUTPUT, 1> reg_in(SHIFT_OUT_UPD, SHIFT_OUT_DATA, SHIFT_OUT_CLK);
GyverShift<INPUT, 2> reg_out(SHIFT_IN_PL, SHIFT_IN_DATA, SHIFT_IN_CLK);

#include "GyverStepper2.h"
GStepper2<STEPPER2WIRE> stepper1(MOTOR1_STEP_PER_ROUND, DRV1_STP, DRV1_DIR);
GStepper2<STEPPER2WIRE> stepper2(MOTOR2_STEP_PER_ROUND, DRV2_STP, DRV2_DIR);
GStepper2<STEPPER2WIRE> stepper3(MOTOR3_STEP_PER_ROUND, DRV3_STP, DRV3_DIR);
GStepper2<STEPPER2WIRE> stepper4(MOTOR4_STEP_PER_ROUND, DRV4_STP, DRV4_DIR);
GStepper2<STEPPER2WIRE> stepper5(MOTOR5_STEP_PER_ROUND, DRV5_STP, DRV5_DIR);

#include "GyverPlanner2.h"
GPlanner2<STEPPER2WIRE, 5> planner;

void setup() {

    // Add steppers to planner
    planner.addStepper(1,stepper1);
    planner.addStepper(2,stepper2);
    planner.addStepper(3,stepper3);
    planner.addStepper(4,stepper4);
    planner.addStepper(5,stepper5);

    // Setup planner
    planner.setMaxSpeed(MAX_PLANNER_SPEED);
    planner.setAcceleration(PLANNER_ACCELERATION);


}

void loop() {

}
