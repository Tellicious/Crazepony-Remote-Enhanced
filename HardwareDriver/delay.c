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
 
 
#include "delay.h"
#include "stm32f10x_it.h"

void delay_ms(uint16_t nms) {
		uint32_t t0 = micros();
		while(micros() - t0 < nms * 1000);
			
}
    								   
void delay_us(uint32_t nus) {
		uint32_t t0 = micros();
		while(micros() - t0 < nus);
			
}
//------------------End of File----------------------------
