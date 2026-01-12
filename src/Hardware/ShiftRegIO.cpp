#include "ShiftRegIO.h"

#include "port.h"
#include "config.h"

GyverShift<OUTPUT, OUTPUT_REG_COUNT> reg_in(SHIFT_OUT_UPD, SHIFT_OUT_DATA, SHIFT_OUT_CLK);
GyverShift<INPUT, INPUT_REG_COUNT> reg_out(SHIFT_IN_PL, SHIFT_IN_DATA, SHIFT_IN_CLK);