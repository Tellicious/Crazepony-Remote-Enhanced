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
*/
/* Control.c file
Author: Andrea Vivani
Date: 2016-06-10
------------------------------------
*/

#include "Config_Params.h"
#include "ADC.h"
#include "Filt.h"
#include "Control.h"
#include "ConfigTable.h"
#include "NRF24.h"
#include "delay.h"
#include "CommUAV.h"
#include "stdio.h"


uint16_t Throttle, Roll, Pitch, Yaw;

int16_t Throttle_Calibra = 0;
int16_t Pitch_Calibra = 0;
int16_t Roll_Calibra = 0;
int16_t Yaw_Calibra = 0;

#define CTRL_DISARMED				0xB6
#define CTRL_ARMED					0x6B
#define CTRL_CONSTRAIN(x)		(x<1000)?1000:((x>2000)?2000:x)


//----------------Load rocker position via ADC---------------------//
void LoadRCdata(void){
	Throttle = Throttle_Calibra - ((float) Get_Adc_Average(3,20) * 1000 / 4096);
	Throttle = CTRL_CONSTRAIN(Throttle);

	Pitch = Pitch_Calibra + ((float) Get_Adc_Average(1,20) * 1000 / 4096);
	Pitch = CTRL_CONSTRAIN(Pitch);

	Roll = Roll_Calibra + ((float) Get_Adc_Average(0,20) * 1000/ 4096);
	Roll = CTRL_CONSTRAIN(Roll);

	Yaw = Yaw_Calibra + ((float) Get_Adc_Average(2,20) * 1000 / 4096);
	Yaw = CTRL_CONSTRAIN(Yaw);  
}


//-------------Calibrate joystick neutral position---------------//
void SetCenterVal(void){
	static uint8_t i;
	uint16_t sum[4] = {0,0,0,0};
	static int8_t lednum = 0;
	static int8_t clibrasumNum = 20;
	uint8_t ledstat = 0;
	
	GPIO_WriteBit(led2, Bit_RESET);
	GPIO_WriteBit(led3, Bit_RESET);
	GPIO_WriteBit(led4, Bit_RESET);
	GPIO_WriteBit(led5, Bit_RESET);

	Throttle_Calibra = 2000;
	Pitch_Calibra    = 1000;
	Roll_Calibra     = 1000;
	Yaw_Calibra      = 1000;
	
	for (i = 0; i < clibrasumNum; i++){
		LoadRCdata();
		sum[0] += Throttle;
		sum[1] += Pitch;
		sum[2] += Roll;
		sum[3] += Yaw;
		delay_ms(100);
		
		if(++lednum == 5)lednum = 1;
		if (lednum == 1){
			GPIO_WriteBit(led5, Bit_RESET);
			GPIO_WriteBit(led2, Bit_SET);
		}
		else if (lednum == 2){
			GPIO_WriteBit(led2, Bit_RESET);
			GPIO_WriteBit(led3, Bit_SET);
		}
		else if (lednum == 3){
		GPIO_WriteBit(led3, Bit_RESET);
		GPIO_WriteBit(led4, Bit_SET);
		}
		else{
		GPIO_WriteBit(led4, Bit_RESET);
		GPIO_WriteBit(led5, Bit_SET);
		}
	}
	
	Throttle_Calibra += 1500 - sum[0] / i;
	Pitch_Calibra    += 1500 - sum[1] / i;
	Roll_Calibra     += 1500 - sum[2] / i;
	Yaw_Calibra      += 1500 - sum[3] / i;


	LoadRCdata();
	if((Throttle >= 1502) || (Throttle <= 1498) || (Pitch >= 1502) || (Pitch <= 1498) || (Roll >= 1502) || (Roll <= 1498) || (Yaw >= 1502) || (Yaw <= 1498)){
		while (1){
			ledstat = !ledstat;
			GPIO_WriteBit(led2, (BitAction) ledstat);
			GPIO_WriteBit(led3, (BitAction) ledstat);
			GPIO_WriteBit(led4, (BitAction) ledstat);
			GPIO_WriteBit(led5, (BitAction) ledstat);
			delay_ms(200);
		}
	}

	GPIO_WriteBit(led2, Bit_RESET);
	GPIO_WriteBit(led3, Bit_RESET);
	GPIO_WriteBit(led4, Bit_RESET);
	GPIO_WriteBit(led5, Bit_RESET);
	SaveParamsToEEPROM();
}	

uint8_t ArmStatus = CTRL_DISARMED;
uint8_t Command = 0;
uint8_t rckCmdCount = 0;

// Command 1 - Disarm
// Throttle Down, Yaw Left
#define DISARM_CHK (Command == 1)?(rckCmdCount++):(rckCmdCount = 1); Command = 1;
// Command 2 - Arm
// Throttle Down, Yaw Right
#define ARM_CHK (Command == 2)?(rckCmdCount++):(rckCmdCount = 1); Command = 2;
// Command 3 - IMU Calibration
// Throttle Down, Yaw Left, Pitch Down, Roll Left
#define IMUCAL_CHK (Command == 3)?(rckCmdCount++):(rckCmdCount = 1); Command = 3;
// Command 4 - RC Calibration
// Throttle Down, Yaw Left, Pitch Down, Roll Right
#define RCCAL_CHK (Command == 4)?(rckCmdCount++):(rckCmdCount = 1); Command = 4;

//------------------Disarm Motors------------------//
void Disarm(void){
	uint8_t i;
	// If disarm for more than 0.4 s, start disarming
	if (rckCmdCount >= 4){
		#ifdef UART_DEBUG
		printf ("Disarming Motors...\r\n");
		#endif
		for(i = 0; i < 5; i++){         
			if(!CommUAVUpload(MSP_DISARM_IT)){
					break;
			}
		}				
		if(i == 5){
			ArmStatus = CTRL_DISARMED; //Disarmed
			GPIO_WriteBit(led5, Bit_RESET);
		#ifdef UART_DEBUG
			printf("Motors disarmed!!!\r\n");
		}
		else {
			printf("Motors cannot be disarmed!!!\r\n");
		#endif
		}
	}
}

//------------------Arm Motors------------------//
void Arm(void){
	uint8_t i;
	// If arm for more than 0.4 s, start arming
	if (rckCmdCount >= 4){
		#ifdef UART_DEBUG
		printf ("Arming Motors...\r\n");
		#endif
		for(i=0;i<5;i++){         
			if(!CommUAVUpload(MSP_ARM_IT)){
				break;
			}	
		}			
		if(i == 5){
			ArmStatus = CTRL_ARMED; //Armed
			GPIO_WriteBit(led5, Bit_SET);
		#ifdef UART_DEBUG
			printf("Motors armed!!!\r\n");
		}

		else {
			printf("Motors cannot be armed!!!\r\n");
		#endif
		}
	}
}

//------------------Calibrate RC Rockers------------------//
void RCCal(void){
	uint8_t led_rccal = 0;
	// When hold for 0.4 s, the calibration starts. If hold for more than that, nothing happens
	if (rckCmdCount == 4){
		// Wait for the rockers to return to central position and blink LED 5
		while ((Get_Adc_Average(3,15) < 1900) || (Get_Adc_Average(3,15) > 2200) || (Get_Adc_Average(0,15) < 1900) || (Get_Adc_Average(0,15) > 2200) || (Get_Adc_Average(1,15) < 1900) || (Get_Adc_Average(1,15) > 2200) || (Get_Adc_Average(2,15) < 1900) || (Get_Adc_Average(2,15) > 2200)){
			GPIO_WriteBit(led5, (BitAction)led_rccal);
			led_rccal = (!led_rccal);
			delay_ms(200);
		}
		delay_ms(200);
		SetCenterVal();
		
		//Re-initialize filters
		filtInit(&filtThrottle);
		filtInit(&filtYaw);
		filtInit(&filtPitch);
		filtInit(&filtRoll);
	}
}

//------------------Calibrate IMU------------------//
void IMUCal(void){
	// When hold for 0.4 s, the IMUcalibration command is sent. If hold for more than that, nothing happens
	if (rckCmdCount == 4){
		#ifdef UART_DEBUG
		printf("IMU Calibration Sent\r\n");
		#endif
		CommUAVUpload(MSP_ACC_CALI);
	}
}

//------------------Commands via Rockers------------------//
void RockersCommands(void){
	if (Throttle < 1050){
		if (Yaw < 1100){
			if ((ArmStatus == CTRL_DISARMED) && (Pitch < 1100)){
				if (Roll > 1900) {
					RCCAL_CHK;
					RCCal();
				}
				else if (Roll < 1100){
					IMUCAL_CHK;
					IMUCal();
				}
			}
			else{
				DISARM_CHK;
				Disarm();
			}
		}
		else if ((Yaw > 1900) && (Roll < 1550) && (Roll > 1450) && (Pitch < 1550) && (Pitch > 1450)){
			ARM_CHK;
			Arm();
		}
		// Overflow check
		if (rckCmdCount >= 200) rckCmdCount = 100;
	}
	else {
		Command = 0;
	}
}
