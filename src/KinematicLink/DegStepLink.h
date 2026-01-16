#pragma once
#include <cstdint>
#include "BaseLink.h"

class DegStepLink : public BaseLink {
private:
    uint16_t steps_per_rev;
    uint16_t deg_per_step;

public:
    explicit DegStepLink(const uint16_t steps_per_rev) : BaseLink(), steps_per_rev(steps_per_rev),
                                                         deg_per_step(360 / steps_per_rev) {
    }

    DegStepLink(const uint16_t steps_per_rev, BaseLink *previos_link) : BaseLink(previos_link),
                                                                        steps_per_rev(steps_per_rev),
                                                                        deg_per_step(360 / steps_per_rev) {
    }

    /**
     * Get rotation degree by stepper steps
     * @param input steps
     * @return degree
     */
    float forward(float input) override {
        return input * deg_per_step;
    };

    /**
     * Get stepper steps by rotation degree
     * @param output degree
     * @return steps
     */
    float backward(float output) override {
        return output / deg_per_step;
    };
};
