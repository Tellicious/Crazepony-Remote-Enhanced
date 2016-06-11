#ifndef _FILT_H_
#define _FILT_H_
#include "stdint.h"

typedef struct{
	uint16_t val_old1;
	uint16_t val_old2;
} filt_vals_t;


extern filt_vals_t filtThrottle;
extern filt_vals_t filtYaw;
extern filt_vals_t filtPitch;
extern filt_vals_t filtRoll;

void filtInit(filt_vals_t* filtVals);
uint16_t filtCompute (filt_vals_t* filtVals, uint16_t cmd_val);
#endif
