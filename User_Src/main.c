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
编写者：小马  (Camel)
作者E-mail：375836945@qq.com
编译环境：MDK-Lite  Version: 4.23
初版时间: 2014-01-28
功能：
------------------------------------
*/
#include "config.h"             //包含所有的驱动头文件

extern char IMUcalibratflag;
extern char Lockflag;

//       Battery=(100*Get_Adc_Average(8,15))/2600;//采集遥控电池电压，赋值飞对应的电池变量
//       Battery=(Battery>=99)?99:Battery; 
int main(void)
{
	static char ledsta;
	static char datasent;
	/***********************************/
	SystemClock_HSI(9);           //系统时钟初始化，时钟源内部HSI
	cycleCounterInit();				    // Init cycle counter
	SysTick_Config(SystemCoreClock / 1000);	//SysTick开启系统tick定时器并初始化其中断，1ms
	UART1_init(SysClock,uart1baudSet); //串口1初始化
  NVIC_INIT();	                //中断初始化
  STMFLASH_Unlock();            //内部flash解锁
  LoadParamsFromEEPROM();				//加载系统参数配置表
  LedInit();	                  //IO初始化
  Adc_Init();										//摇杆AD初始化
	KeyInit();										//按键初始化
	NRF24_config(40, NRF24_PA_MAX, NRF24_2MBPS, 32, NRF24_DIS_DYN_PYL, NRF24_ADDR_5, NRF24_CRC_16, NRF24_EN_AUT_ACK, NRF24_ARD_500, NRF24_ARC_9);
	NRF24_reopenWritingPipe();
	NRF24_startTransmitter();
  controlClibra();							//遥控摇杆校准
#ifdef UART_DEBUG  
	TIM3_Init(SysClock,2000);			//定时器初始化，1s为周期打印摇杆值
#endif
	TIM4_Init(SysClock,TIME4_Preiod);	  //定时器4初始化，定时时间单位：(TIME4_Preiod)微秒
	
	LedSet(led2,1);
	LedSet(led3,1);
	               
  Lockflag = 0;								 
	
  LedSet(led2,0);
	LedSet(led3,0);
	 
  while (1)             
	{ 
		//10Hz loop
		if(flag10Hz == 1)  //10Hz 
		{		
			flag10Hz = 0;
			/*status led*/
			datasent?(NRF24_checkRPD()?(ledsta = 1): (ledsta = !ledsta)): (ledsta = 0);
			LedSet(signalLED, ledsta);				        
			/*crazepony Lock*/
			//KeyArmDisarmCrazepony();
			RockerArmDisarmCrazepony();
			/*IMUcalibrate  */
			IMUcalibrate();
			/*remote calibrate*/
			RemoteCalibrate();
		}

		//50Hz loop
		if(flag50Hz == 1)
		{
			//LoadRCdata();
			flag50Hz = 0;
			
		}
		
		// 80Hz 12.5ms
		if(flag80Hz)
		{
			flag80Hz = 0;
			LoadRCdata();
			//Filter input
			filtComputeAll();
			(CommUAVUpload(MSP_SET_4CON))?(datasent = 1):(datasent = 0);   
		}
	}
}

