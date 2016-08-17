#include "Config_Params.h"
#include "stm32f10x.h"
#include "stdio.h"
#include "ADC.h"
#include "NRF24.h"
#include "UART1.h"

uint16_t flag10Hzcnt,flag50Hzcnt,flag80Hzcnt,flag100Hzcnt;

uint16_t flag10Hz,flag50Hz,flag80Hz,flag100Hz;

void TIM4_IRQHandler(void) {
    if( TIM_GetITStatus(TIM4 , TIM_IT_Update) != RESET ) {     
					if(++flag10Hzcnt >= 200){//10Hz
									flag10Hzcnt = 0;
									flag10Hz = 1;
								} 
					if(++flag50Hzcnt >= 40){//50Hz
									flag50Hzcnt = 0;
									flag50Hz = 1;
								}   
					if(++flag80Hzcnt >= 25){ //80Hz
									flag80Hzcnt = 0;
									flag80Hz = 1;
								}
					if(++flag100Hzcnt >= 20){ //100Hz
									flag100Hzcnt = 0;
									flag100Hz = 1;
								}	

          TIM_ClearITPendingBit(TIM4 , TIM_IT_Update); 
    }
}

#ifdef UART_DEBUG
void TIM3_IRQHandler(void) {
    if( TIM_GetITStatus(TIM3 , TIM_IT_Update) != RESET ) {     
				printf("thr -->%d\r\n",Throttle);
				printf("pitch -->%d\r\n",Pitch);
				printf("roll -->%d\r\n",Roll);
				printf("yaw -->%d\r\n",Yaw);
				printf("Battery (V) -->%f\r\n", GetBattVolt());
				printf("remote addr -->0x%x%x%x%x%x\r\n", NRF24_tx_address[4], NRF24_tx_address[3], NRF24_tx_address[2], NRF24_tx_address[1], NRF24_tx_address[0]);// tx addr
				printf("-------------\r\n");
        TIM_ClearITPendingBit(TIM3 , TIM_IT_Update);
    }
}
#endif

// Key Mode
void EXTI1_IRQHandler(void){
	if(EXTI_GetITStatus(EXTI_Line1) != RESET) {
		 #ifdef UART_DEBUG
	    	printf("key mode press...\r\n");
		 #endif
		
		NRF24_RandTXAddr();

   	EXTI_ClearITPendingBit(EXTI_Line1); 
	}
}

// Key "+"
void EXTI3_IRQHandler(void){
	if(EXTI_GetITStatus(EXTI_Line3) != RESET) {
		 #ifdef UART_DEBUG
	    	printf("key add press...\r\n");
		 #endif

		EXTI_ClearITPendingBit(EXTI_Line3);
		
	}
}

// Key "-"
void EXTI9_5_IRQHandler(void){
	if(EXTI_GetITStatus(EXTI_Line8) != RESET) {	    		
		 #ifdef UART_DEBUG
	    	printf("key sub press...\r\n");
		 #endif

		EXTI_ClearITPendingBit(EXTI_Line8);
	}
}

// UART1 Interrupt
#ifdef UART1_USE_INTERRUPTS
volatile uint8_t Udatatmp;
void USART1_IRQHandler(void) {
	if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET){   
		USART_SendData(USART1, UartBuf_RD(&UartTxbuf));
		if(UartBuf_Cnt(&UartTxbuf) == 0)  
			USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
	}
	else if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		Udatatmp = (uint8_t) USART_ReceiveData(USART1);
		UartBuf_WD(&UartRxbuf,Udatatmp);
	}
}
#endif
