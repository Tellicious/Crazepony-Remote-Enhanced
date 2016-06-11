#ifndef _CONTROL_H_
#define _CONTROL_H_
#include "stm32f10x.h"

void LoadRCdata(void);
void controlClibra(void);
void RockerArmDisarmCrazepony(void);
void KeyArmDisarmCrazepony(void);
void IMUcalibrate(void);
void Remotecalibrate(void);


extern uint16_t Throttle;
extern uint16_t Roll;
extern uint16_t Pitch;
extern uint16_t Yaw;
extern int8_t ClibraFlag;


#endif


