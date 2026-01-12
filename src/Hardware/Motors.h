#pragma once

#include "GyverStepper2.h"
#include "GyverPlanner2.h"

namespace Motors {
    extern GStepper2<STEPPER2WIRE> steppers[5];
    extern GPlanner2<STEPPER2WIRE, 5> planner;
    void setup();
}
