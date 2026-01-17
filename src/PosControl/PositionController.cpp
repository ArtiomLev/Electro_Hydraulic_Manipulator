#include "PositionController.h"

#include "../Hardware/Motors.h"

#include "KinematicLink/KinematicLink.h"
MultistepLink multistep_link(16);
DegStepLink deg_step_link(200, &multistep_link);
ScrewLink screw_link(2, &deg_step_link);

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
    for (auto &pos: positions)
        pos = 0.0f;
    homing = true;
}

bool PositionController::isReset() const {
    return homing;
}

void PositionController::setAxisPos(const uint8_t axis) const {
    const int32_t stepper_pos = static_cast<int32_t>(screw_link.backward(positions[axis]));
    Motors::steppers[axis].setTarget(stepper_pos * (invert[axis]? -1 : 1));
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

PositionController pos_control;
