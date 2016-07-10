#ifndef _CONTROL_H_
#define _CONTROL_H_
#include "stm32f10x.h"

void LoadRCdata(void);
void SetCenterVal(void);
void RockerArmDisarmCrazepony(void);
//void KeyArmDisarmCrazepony(void);
void RockersIMUCalibrate(void);
void RockersRCCalibrate(void);
//void RockersCommands(void);

#endif


