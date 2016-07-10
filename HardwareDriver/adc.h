#ifndef __ADC_H
#define __ADC_H	
#include "stdint.h"
uint16_t  Get_Adc(uint8_t ch); 
uint16_t Get_Adc_Average(uint8_t ch,uint8_t times); 
int Get_Temp(void);
float GetBattVolt(void);
uint8_t GetBattVal(void);
#endif 















