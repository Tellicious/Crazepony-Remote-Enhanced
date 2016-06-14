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

#include "config.h"
#include "adc.h"
#include "Control.h"
#include "ConfigTable.h"
#include "Led.h"
#include "NRF24.h"
#include "delay.h"
#include "CommUAV.h"


uint16_t Throttle, Roll, Pitch, Yaw;

int16_t Throttle_Calibra=0;
int16_t Pitch_Calibra   =0;
int16_t Roll_Calibra    =0;
int16_t Yaw_Calibra     =0;//摇杆校准值

char Lockflag      = 0;
char IMUcalibratflag = 0;

#define CTRL_DISARMED				0xB6
#define CTRL_ARMED					0x6B
#define CTRL_CONSTRAIN(x)		(x<1000)?1000:((x>2000)?2000:x)


//----------------Load rocker position via ADC---------------------//
void LoadRCdata(void){
	Throttle = Throttle_Calibra - ((float) Get_Adc_Average(3,15) * 0.244140625);
	Throttle = CTRL_CONSTRAIN(Throttle);

	Pitch = Pitch_Calibra + ((float) Get_Adc_Average(1,15) * 0.244140625);
	Pitch = CTRL_CONSTRAIN(Pitch);

	Roll = Roll_Calibra + ((float) Get_Adc_Average(0,15) * 0.244140625);
	Roll = CTRL_CONSTRAIN(Roll);

	Yaw = Yaw_Calibra + ((float) Get_Adc_Average(2,15) * 0.244140625);
	Yaw = CTRL_CONSTRAIN(Yaw);  
}


//-------------Calibrate joystick neutral position---------------//
int8_t ClibraFlag;

void controlClibra(void){
	static u8 i;
	uint16_t sum[4] = {0,0,0,0};
	static int8_t lednum = 1;
  static int8_t clibrasumNum = 20;

	if(ClibraFlag == FAIL){
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
			
			if(++lednum == led5 + 2)lednum = 2;
			LedSet(lednum - 1,0);
			LedSet(lednum ,1);
		}
		
		Throttle_Calibra += 1500 - sum[0] / i;
		Pitch_Calibra    += 1500 - sum[1] / i;
		Roll_Calibra     += 1500 - sum[2] / i;
		Yaw_Calibra      += 1500 - sum[3] / i;


		LoadRCdata();
		if((Throttle >= 1510) || (Throttle <= 1490) || (Pitch >= 1510) || (Pitch <= 1490) || (Roll >= 1510) || (Roll <= 1490) || (Yaw >= 1510) || (Yaw <= 1490))
						ClibraFlag = FAIL;
		else {		
			ClibraFlag = OK;
			SaveParamsToEEPROM();
		}
		LedSet(led2,0);
		LedSet(led3,0);
		LedSet(led4,0);
		LedSet(led5,0);
	 }	
}


uint8_t ArmStatus = CTRL_DISARMED;


//--------------Arm and disarm motors via "+" key----------------//
static u8 cnt = 3;

void KeyArmDisarmCrazepony(void)
{
	u8 i;
	
	if(0 == ++cnt){
		//防止数据溢出
		cnt = 3;
	}
	
	switch(Lockflag)
	{
		case 1:
				//解决按键连按的毛刺
				//本函数会被主循环10Hz调用，测试发现cnt为1或者2的时候，（200ms以下），属于按键毛刺，应该剔除
				if(cnt < 3){
					printf("invalid key press:%d\n",cnt);
					cnt = 0;
					Lockflag = 0;
					break;
				}else{
					cnt = 0;
				}
		
				if(ArmStatus == CTRL_DISARMED) 
				{
					for(i=0;i<5;i++)         
					CommUAVUpload(MSP_ARM_IT);   //unlock Crazepony
					ArmStatus = CTRL_ARMED;
					Lockflag = 0;
				}
					
				else if(ArmStatus == CTRL_ARMED)
				{
					for(i=0;i<5;i++)         
					CommUAVUpload(MSP_DISARM_IT);	//lock Crazepony
					ArmStatus = CTRL_DISARMED;
					Lockflag = 0;
				}
			break;
		case 0:
			if(ArmStatus == CTRL_ARMED)   LedSet(led5,1);
			else if(ArmStatus == CTRL_DISARMED) LedSet(led5,0);
			break;
	}	
}

//----------------Arm and disarm motors via rocker-------------------//
uint8_t cnt_arm = 0;
uint8_t cnt_disarm = 0;

void RockerArmDisarmCrazepony(void){
	uint8_t i;
	// Disarm: LH rocker bottom left position
	if((Throttle < 1050) && (Yaw < 1100)){
		cnt_arm = 0;
		cnt_disarm++;
		// If disarm for more than 0.4 s, start disarming
		if (cnt_disarm >= 4){
			cnt_disarm = 4;
			#ifdef UART_DEBUG
			printf ("Disarming Motors...\r\n");
			#endif
			for(i=0;i<4;i++){         
				CommUAVUpload(MSP_DISARM_IT);	
			}				
			if(CommUAVUpload(MSP_DISARM_IT)){
				ArmStatus = CTRL_DISARMED; //Disarmed
				LedSet(led5,0);
			#ifdef UART_DEBUG
				printf("Motors disarmed!!!\r\n");
			}
			else {
				printf("Motors cannot be disarmed!!!\r\n");
			#endif
			}
		}
	}
	// Arm: LH rocker bottom right position
	else if((Throttle < 1050) && (Yaw > 1900)){
		cnt_arm++;
		cnt_disarm = 0;
		// If arm for more than 0.4 s, start arming
		if (cnt_arm >= 4){
			cnt_arm = 4;
			#ifdef UART_DEBUG
			printf ("Arming Motors...\r\n");
			#endif
			for(i=0;i<4;i++){         
				CommUAVUpload(MSP_ARM_IT);	
			}			
			if(CommUAVUpload(MSP_ARM_IT)){
				ArmStatus = CTRL_ARMED; //Armed
				LedSet(led5,1);
			#ifdef UART_DEBUG
				printf("Motors armed!!!\r\n");
			}

			else {
				printf("Motors cannot be armed!!!\r\n");
			#endif
			}
		}
	}
	else{
		cnt_arm = 0;
		cnt_disarm = 0;
	}
}

/*IMUcalibrate  */
void IMUcalibrate(void)
{
	  LedSet(led4,IMUcalibratflag);
	  if(IMUcalibratflag) 
			{
				CommUAVUpload(MSP_ACC_CALI);
				IMUcalibratflag = 0;
			}
}


//----------------Calibrate remote via rocker-------------------//

uint8_t cnt_rcal = 0;

void RemoteCalibrate(void){
	// Calibrate mode starts when holding LH rocker in bottom left position and RH rocker in bottom right position
	uint8_t led_rccal = 0;
	if ((ArmStatus == CTRL_DISARMED) && (Throttle < 1050) && (Yaw < 1100) && (Pitch < 1100)&& (Roll > 1900)){
		cnt_rcal++;
		// When hold for 0.4 s, the calibration starts. If hold for more than that, nothing happens
		if (cnt_rcal == 4){
			ClibraFlag = FAIL;
			// Wait for the rockers to return to central position and blink LED 5
			while ((Get_Adc_Average(3,15) < 1900) || (Get_Adc_Average(3,15) > 2200) || (Get_Adc_Average(0,15) < 1900) || (Get_Adc_Average(0,15) > 2200) || (Get_Adc_Average(1,15) < 1900) || (Get_Adc_Average(1,15) > 2200) || (Get_Adc_Average(2,15) < 1900) || (Get_Adc_Average(2,15) > 2200)){
				LedSet(led5, led_rccal);
				led_rccal = (!led_rccal);
				delay_ms(200);
			}
			delay_ms(200);
			controlClibra();
			
            //Re-initialize filters
            filtInit(&filtThrottle);
            filtInit(&filtYaw);
            filtInit(&filtPitch);
            filtInit(&filtRoll);
		}
	}
	else{
		cnt_rcal = 0;
	}
}
