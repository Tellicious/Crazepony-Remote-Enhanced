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
main.c file

------------------------------------
*/        
#include "stm32f10x.h"
#include "Config_Params.h"
#include "ADC.h"
#include "Control.h"
#include "Init_Config.h"
#include "Filt.h"
#include "NRF24.h"
#include "CommUAV.h"
#include "stmflash.h"
#include "ConfigTable.h"

int main(void) {
	static uint8_t datasent;
	/***********************************/
	HardwareInit();
	STMFLASH_Unlock();            
  LoadParamsFromEEPROM();
	NRF24_config(40, NRF24_PA_MAX, NRF24_2MBPS, 32, NRF24_DIS_DYN_PYL, NRF24_ADDR_5, NRF24_CRC_16, NRF24_EN_AUT_ACK, NRF24_ARD_500, NRF24_ARC_15);
	NRF24_reopenWritingPipe();
	NRF24_startTransmitter();
	SetCenterVal();	

  while (1){ 
		//80Hz loop
		if(flag80Hz) {
			flag80Hz = 0;
			LoadRCdata();
			//Filter input
			filtComputeAll();
			(CommUAVUpload(MSP_SET_4CON))?(datasent = 1):(datasent = 0);   
		}
		
		//10Hz loop
		if(flag10Hz == 1) {		
			flag10Hz = 0;
			/*status led*/
			GPIO_WriteBit(signalLED, (BitAction) (datasent ? (NRF24_checkRPD() ? 1: NOT_signalLED) : 0));
			/*crazepony Lock*/
			RockerArmDisarmCrazepony();
			/*IMUcalibrate  */
			RockersIMUCalibrate();
			/*remote calibrate*/
			RockersRCCalibrate();
			/*Battery level check*/
			GPIO_WriteBit(led4, (BitAction) ((GetBattVal() <= 1) ? NOT_led4 : 1));
		}

		//50Hz loop
		if(flag50Hz == 1) {
			flag50Hz = 0;
		}
	}
}

