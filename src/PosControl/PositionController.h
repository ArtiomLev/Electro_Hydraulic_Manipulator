#pragma once
#include <cstdint>

class PositionController {
private:
    bool homing = false;
    float positions[5];
    bool invert[5];

    void setAxisPos(uint8_t axis) const;

public:
    PositionController();

    void reset();

    bool isReset() const;

    void axisGoTo(uint8_t axis, float position);

    void axisGoToRel(uint8_t axis, float position);

    float getPosition(uint8_t axis);

    void brake();

    static bool axisIdle(uint8_t axis);

    static bool systemIdle();
};

extern PositionController pos_control;