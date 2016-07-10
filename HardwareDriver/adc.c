/*    
      ____                      _____                  +---+
     / ___\                     / __ \                 | R |
    / /                        / /_/ /                 +---+
   / /   ________  ____  ___  / ____/___  ____  __   __
  / /  / ___/ __ `/_  / / _ \/ /   / __ \/ _  \/ /  / /
 / /__/ /  / /_/ / / /_/  __/ /   / /_/ / / / / /__/ /
 \___/_/   \__,_/ /___/\___/_/    \___ /_/ /_/____  /
                                                 / /
                                            ____/ /
                                           /_____/
------------------------------------
*/
#include "ADC.h"
#include "stm32f10x.h"

uint16_t Get_Adc(uint8_t ch) {
 		 
	ADC1->SQR3&=0XFFFFFFE0;
	ADC1->SQR3|=ch;		  			    
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	while(!(ADC1->SR&1<<1)); 	   
	return (uint16_t) ADC1->DR;
}

uint16_t Get_Adc_Average(uint8_t ch,uint8_t times) {
	u32 temp_val = 0;
 	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val += Get_Adc(ch);
	}
	return temp_val/times;
} 
	 
int Get_Temp(void) {				 
	float temperature;   
	temperature=(float)Get_Adc_Average(16, 20) * 16.11328125e-4;
	temperature=(1.43 - temperature) / 0.0043 + 25;
	temperature *= 10;
	return (int)temperature;	 
}

float GetBattVolt(void){	
	return Get_Adc_Average(8, 20) * 16.11328125e-4;
}

uint8_t GetBattVal(void){
	float BattVolt = GetBattVolt();
	if (BattVolt >= 4.0) return 6;
	else if (BattVolt >= 3.9) return 5;
	else if (BattVolt >= 3.84) return 4;
	else if (BattVolt >= 3.8) return 3;
	else if (BattVolt >= 3.75) return 2;
	else if (BattVolt >= 3.73) return 1;
	else return 0;
}







