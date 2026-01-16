#pragma once
#include "BaseLink.h"
#include <cstdint>

class MultistepLink : public BaseLink {
protected:
    const uint8_t multiplier;

public:
    explicit MultistepLink(const uint8_t multiplier) : BaseLink(), multiplier(multiplier) {
    };

    MultistepLink(const uint8_t multiplier, BaseLink *previosLink) : BaseLink(previosLink), multiplier(multiplier) {
    };

    float forward(const float input) override {
        return input * multiplier;
    };

    float backward(const float output) override {
        return output / multiplier;
    }

    uint8_t getMultiplier() const { return multiplier; }
};
