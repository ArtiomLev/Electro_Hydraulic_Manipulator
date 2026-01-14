#pragma once

#include "EncButton.h"

class Endstops {
private:
    VirtButton endstops[5][2];

public:
    Endstops();

    enum EndstopDirection: bool {
        POSITIVE = 0,
        NEGATIVE = 1
    };

    VirtButton& getEndstop(uint8_t axis, EndstopDirection direction);

    void tick();

};

extern Endstops endstops;