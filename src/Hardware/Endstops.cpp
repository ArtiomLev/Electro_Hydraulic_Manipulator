#include "Endstops.h"

#include "ShiftRegIO.h"

Endstops endstops;

Endstops::Endstops() : endstops{} {
    for (auto axis_endstops: endstops) {
        axis_endstops[NEGATIVE].setBtnLevel(HIGH);
        axis_endstops[POSITIVE].setBtnLevel(HIGH);
    }
}

VirtButton &Endstops::getEndstop(uint8_t axis, EndstopDirection direction) {
    return endstops[axis - 1][direction];
}

void Endstops::tick() {
    reg_in.update();
    uint8_t i = 0;
    for (auto axis_endstops: endstops) {
        axis_endstops[NEGATIVE].tick(reg_in[i++]);
        axis_endstops[POSITIVE].tick(reg_in[i++]);
    }
}
