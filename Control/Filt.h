#ifndef _FILT_H_
#define _FILT_H_
#include "stdint.h"

typedef struct{
	uint16_t u_old_1;
	uint16_t u_old_2;
	float f_old_1;
	float f_old_2;
} filt_vals_t;

extern filt_vals_t filtThrottle, filtYaw, filtPitch, filtRoll;

void filtInit(filt_vals_t* filtVals);
void filtCompute (filt_vals_t* filtVals, uint16_t* u);
void filtComputeAll(void);
#endif
