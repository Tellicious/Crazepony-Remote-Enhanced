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
			for(i=0;i<5;i++){         
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
	else{
		cnt_arm = 0;
		cnt_disarm = 0;
	}
}

//----------------Calibrate IMU via rocker-------------------//

uint8_t cnt_IMUcal = 0;

void RockersIMUCalibrate(void){
	// Calibrate mode starts when holding LH rocker in bottom left position and RH rocker in bottom right position
	if ((ArmStatus == CTRL_DISARMED) && (Throttle < 1050) && (Yaw < 1100) && (Pitch < 1100)&& (Roll < 1100)){
		if(0 == ++cnt_IMUcal){
		// this to prevent overflow
		cnt_IMUcal = 5;
		}
		// When hold for 0.4 s, the IMUcalibration command is sent. If hold for more than that, nothing happens
		if (cnt_IMUcal == 4){
			printf("azz\r\n");
			CommUAVUpload(MSP_ACC_CALI);
		}
	}
	else{
		cnt_IMUcal = 0;
	}
}

//----------------Calibrate remote via rocker-------------------//

uint8_t cnt_rcal = 0;

void RockersRCCalibrate(void){
	// Calibrate mode starts when holding LH rocker in bottom left position and RH rocker in bottom right position
	uint8_t led_rccal = 0;
	if ((ArmStatus == CTRL_DISARMED) && (Throttle < 1050) && (Yaw < 1100) && (Pitch < 1100)&& (Roll > 1900)){
		if(0 == ++cnt_rcal){
		// this to prevent overflow
		cnt_rcal = 5;
		}
		// When hold for 0.4 s, the calibration starts. If hold for more than that, nothing happens
		if (cnt_rcal == 4){
			// Wait for the rockers to return to central position and blink LED 5
			while ((Get_Adc_Average(3,15) < 1900) || (Get_Adc_Average(3,15) > 2200) || (Get_Adc_Average(0,15) < 1900) || (Get_Adc_Average(0,15) > 2200) || (Get_Adc_Average(1,15) < 1900) || (Get_Adc_Average(1,15) > 2200) || (Get_Adc_Average(2,15) < 1900) || (Get_Adc_Average(2,15) > 2200)){
				GPIO_WriteBit(led5, (BitAction)led_rccal);
				led_rccal = !led_rccal;
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
	else{
		cnt_rcal = 0;
	}
}