#include "PositionController.h"
#include "config.h"

#include "../Hardware/Motors.h"

#include "KinematicLink/KinematicLink.h"
MultistepLink multistep_link(DRV_MICROSTEPPING);
DegStepLink deg_step_link(200, &multistep_link);
ScrewLink screw_link(0.7, &deg_step_link);

PositionController::PositionController() : positions{
                                               0,
                                               0,
                                               0,
                                               0,
                                               0
                                           },
                                           invert{
                                               true,
                                               true,
                                               false,
                                               false,
                                               true
                                           } {
}

void PositionController::reset() {
    for (uint8_t i = 1 ; i <= 5 ; i++) {
        positions[i] = 0;
        Motors::steppers[i].reset();
        Motors::steppers[i].setCurrent(0);
    }
    homing = true;
}

bool PositionController::isReset() const {
    return homing;
}

void PositionController::setAxisPos(const uint8_t axis) const {
    const float pos = positions[axis];
    const int32_t stepper_pos = static_cast<int32_t>(screw_link.chainBackward(pos));
    Motors::steppers[axis].setTarget(stepper_pos * (invert[axis] ? -1 : 1));
    Serial.printf("\nFor axis %d set target %f mm = %d stp\n", axis, pos, stepper_pos);
}

void PositionController::axisGoTo(uint8_t axis, const float position) {
    if (!isReset()) reset();
    axis--;
    if (axis < 0 || axis >= 5) return;
    positions[axis] = position;
    setAxisPos(axis);
}

void PositionController::axisGoToRel(uint8_t axis, float distance) {
    if (!isReset()) reset();
    axis--;
    if (axis < 0 || axis >= 5) return;
    positions[axis] += distance;
    setAxisPos(axis);
}

float PositionController::getPosition(uint8_t axis) {
    if (!isReset()) reset();
    axis--;
    if (axis < 0 || axis >= 5) return 0.0f;
    return positions[axis];
}

void PositionController::brake() {
    for (auto stepper: Motors::steppers)
        stepper.brake();
    homing = false;
}

bool PositionController::axisIdle(uint8_t axis) {
    axis--;
    if (axis < 0 || axis >= 5) return true;
    return Motors::steppers[axis].getStatus() == 0;
}

bool PositionController::systemIdle() {
    for (uint8_t i = 1 ; i <= 5 ; i++) {
        if (!axisIdle(i)) return false;
    }
    return true;
}

PositionController pos_control;
