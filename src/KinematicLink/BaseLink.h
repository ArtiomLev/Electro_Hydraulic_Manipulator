#pragma once

/**
 * Default base kinematic link virtual class
 */
class BaseLink {
private:
    // Pointer to previos link
    BaseLink *previosLink = nullptr;

public:
    /**
     * Initialize first link
    */
    BaseLink() {
    };

    /**
     * Initialize link in chain
     * @param previos_link
     */
    explicit BaseLink(BaseLink *previos_link) : previosLink(previos_link) {
    };

    virtual ~BaseLink() = default;

    /**
     * Method to get value you got through this link
     * @param input
     * @return Output value
     */
    virtual float forward(float input) = 0;

    /**
     * Method to get value you should put to link input to get output
     * @param output
     * @return Input value
     */
    virtual float backward(float output) = 0;

    /**
     * Get value you get from system if put input vlue to system input
     * @param initial_input
     * @return System output
     */
    float chainForward(float initial_input) {
        if (previosLink) {
            return forward(previosLink->chainForward(initial_input));
        }
        return forward(initial_input);
    }

    /**
     * Got value you should to put on system input to get output
     * @param final_output
     * @return System input
     */
    float chainBackward(float final_output) {
        const float current_input = backward(final_output);
        if (previosLink) {
            return previosLink->chainBackward(current_input);
        }
        return current_input;
    }
};
