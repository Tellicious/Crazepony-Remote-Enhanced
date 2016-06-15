#include "Filt.h"
#include "stdint.h"
#include "config.h"

// FIR
/*float filt_c1 = 0.22223;  //Cut-off around 3 Hz with an 80Hz loop
float filt_c2 = 0.55554;
float filt_c3 = 0.22223;
*/

// Butterworth with cut-off at 3 Hz
/*float filt_n1 = 0.01186;
float filt_n2 = 0.02372;
float filt_n3 = 0.01186;
float filt_d2 = -1.669;
float filt_d3 = 0.7166;*/


// Second order LP filter with cut-off at 3 Hz
float filt_n1 = 0.018429487403970;
float filt_n2 = 0.036858974807939;
float filt_n3 = 0.018429487403970;
float filt_d2 = -1.456979007345467;
float filt_d3 = 0.530696956961346;


filt_vals_t filtThrottle, filtYaw, filtPitch, filtRoll;

void filtInit(filt_vals_t* filtVals){
	filtVals -> u_old_1 = 1500;
	filtVals -> u_old_2 = 1500;
	filtVals -> f_old_1 = 1500;
	filtVals -> f_old_2 = 1500;
}

void filtCompute (filt_vals_t* filtVals, uint16_t* u){
	float output = filt_n1 * *u + filt_n2 * filtVals -> u_old_1 + filt_n3 * filtVals -> u_old_2 - filt_d2 * filtVals -> f_old_1 - filt_d3 * filtVals -> f_old_2;
	filtVals -> u_old_2 = filtVals -> u_old_1;
	filtVals -> u_old_1 = *u;
	filtVals -> f_old_2 = filtVals -> f_old_1;
	filtVals -> f_old_1 = output;
	*u = output;
}

void filtComputeAll(void){
	filtCompute(&filtThrottle, &Throttle);
	filtCompute(&filtYaw, &Yaw);
	filtCompute(&filtPitch, &Pitch);
	filtCompute(&filtRoll, &Roll);
}
