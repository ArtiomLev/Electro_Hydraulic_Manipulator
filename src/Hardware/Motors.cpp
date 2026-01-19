#include "Motors.h"

#include "port.h"
#include "config.h"

GStepper2<STEPPER2WIRE> Motors::steppers[5] = {
    GStepper2<STEPPER2WIRE>(MOTOR1_STEP_PER_ROUND * DRV1_MICROSTEPPING, DRV1_STP, DRV1_DIR),
    GStepper2<STEPPER2WIRE>(MOTOR2_STEP_PER_ROUND * DRV2_MICROSTEPPING, DRV2_STP, DRV2_DIR),
    GStepper2<STEPPER2WIRE>(MOTOR3_STEP_PER_ROUND * DRV3_MICROSTEPPING, DRV3_STP, DRV3_DIR),
    GStepper2<STEPPER2WIRE>(MOTOR4_STEP_PER_ROUND * DRV4_MICROSTEPPING, DRV4_STP, DRV4_DIR),
    GStepper2<STEPPER2WIRE>(MOTOR5_STEP_PER_ROUND * DRV5_MICROSTEPPING, DRV5_STP, DRV5_DIR),
};

// GPlanner2<STEPPER2WIRE, 5> Motors::planner;

void Motors::setup() {
    pinMode(DRV_EN, OUTPUT);
    digitalWrite(DRV_EN, LOW);

    // Add steppers to planner
    // uint8_t i = 1;
    // for (auto stepper: steppers)
    //     planner.addStepper(i++, stepper);

    // Setup planner
    // planner.setMaxSpeed(MAX_PLANNER_SPEED);
    // planner.setAcceleration(PLANNER_ACCELERATION);

    for (auto &stepper: steppers) {
        stepper.setMaxSpeed(MAX_PLANNER_SPEED);
        stepper.setAcceleration(PLANNER_ACCELERATION);
    }
}

void Motors::tick() {
    // planner.tick();
    for (auto &stepper: steppers)
        stepper.tick();
}
