#pragma once
#include "BaseLink.h"

/**
 * Get distance from rotation
 */
class ScrewLink : public BaseLink {
private:
    // Screw step in mm/rev
    float screw_step;
    // Ratio of degrees of rotation to distance
    float mm_deg;

public:
    explicit ScrewLink(const float screw_step) : BaseLink(), screw_step(screw_step), mm_deg(screw_step / 360.0f) {
    };

    ScrewLink(const float screw_step, BaseLink *previos_link) : BaseLink(previos_link), screw_step(screw_step),
                                                                mm_deg(screw_step / 360.0f) {
    };

    /**
     * Get distance by rotation degree
     * @param input degree
     * @return distance
     */
    float forward(const float input) override {
        return input * mm_deg;
    };

    /**
     * Get rotation degree by distance
     * @param output distance
     * @return degree
     */
    float backward(const float output) override {
        return output / mm_deg;
    };
};
