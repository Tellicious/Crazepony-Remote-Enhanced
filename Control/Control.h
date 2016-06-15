#ifndef _CONTROL_H_
#define _CONTROL_H_
#include "stm32f10x.h"

void LoadRCdata(void);
void controlClibra(void);
void RockerArmDisarmCrazepony(void);
//void KeyArmDisarmCrazepony(void);
void IMUcalibrate(void);
void RemoteCalibrate(void);


extern uint16_t Throttle, Roll, Pitch, Yaw;
	
extern int8_t ClibraFlag;
extern uint8_t ArmStatus;

#endif


