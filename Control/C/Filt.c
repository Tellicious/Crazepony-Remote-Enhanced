#include "Filt.h"
#include "stdint.h"

float filt_c1 = 0.22223;  //Cut-off around 3 Hz with an 80Hz loop
float filt_c2 = 0.55554;
float filt_c3 = 0.22223;

filt_vals_t filtThrottle;
filt_vals_t filtYaw;
filt_vals_t filtPitch;
filt_vals_t filtRoll;

void filtInit(filt_vals_t* filtVals){
	filtVals -> val_old1 = 1500;
	filtVals -> val_old2 = 1500;
}

uint16_t filtCompute (filt_vals_t* filtVals, uint16_t cmd_val){
	uint16_t output = (float) (filt_c1 * cmd_val + filt_c2 * filtVals -> val_old1 + filt_c3 * filtVals -> val_old2);
	filtVals -> val_old2 = filtVals -> val_old1;
	filtVals -> val_old1 = cmd_val;
	return output;
}

