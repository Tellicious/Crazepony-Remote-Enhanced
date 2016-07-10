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
UART1.c file
------------------------------------
*/
#include "UART1.h"
#include "Config_Params.h"
#include "stdio.h"
#include "stm32f10x.h"

#ifdef UART1_USE_INTERRUPTS
//Ring Buffers
UartBuf UartTxbuf;
UartBuf UartRxbuf;
unsigned char rx_buffer[RX_BUFFER_SIZE];
unsigned char tx_buffer[TX_BUFFER_SIZE];
#endif

//////////////////////////////////////////////////////////////////
// Add the following code to support the printf function	  
#ifdef UART1_STDIO_FUN
#pragma import(__use_no_semihosting)                          
struct __FILE {int handle;/* Whatever you require here*/};
 
FILE __stdout;
FILE __stdin;
       
int fputc(int ch, FILE *f) {   
	#ifdef UART1_NO_INTERRUPTS
	while (!(USART1->SR & USART_FLAG_TXE));//wait for TXE (TX Empty flag)      
	USART1->DR = (ch & 0x1FF);
	#elif defined(UART1_USE_INTERRUPTS)
	UartPutCh((uint8_t) ch);
	#endif
	return ch;
}

int fgetc (FILE *f) {
	#ifdef UART1_NO_INTERRUPTS
	while (!(USART1->SR & USART_FLAG_RXNE));
	return ((int)(USART1->DR & 0x1FF));
	#elif defined(UART1_USE_INTERRUPTS)
	return ((int) UartBuf_RD(&UartRxbuf));
	#endif
}

void _ttywrch(int ch) {
	#ifdef UART1_NO_INTERRUPTS
	while (!(USART1->SR & USART_FLAG_TXE));//wait for TXE (TX Empty flag)      
	USART1->DR = (ch & 0x1FF);
	#elif defined(UART1_USE_INTERRUPTS)
	UartPutCh((uint8_t) ch);
	#endif
}

//Define _sys_exit () to avoid using semihosting mode   
void _sys_exit(int x) { 
	x = x; 
} 
#endif 

#ifdef UART1_USE_INTERRUPTS
uint8_t UartBuf_RD(UartBuf *Ringbuf) {
	uint8_t temp;
	temp = Ringbuf->pbuf[Ringbuf->Rd_Indx & Ringbuf->Mask];
	Ringbuf->Rd_Indx++;
	return temp;
}

void UartBuf_WD(UartBuf *Ringbuf, uint8_t DataIn) {
	Ringbuf->pbuf[Ringbuf->Wd_Indx & Ringbuf->Mask] = DataIn;
	Ringbuf->Wd_Indx++;

}

uint16_t UartBuf_Cnt(UartBuf *Ringbuf) {
	return (Ringbuf->Wd_Indx - Ringbuf->Rd_Indx) & Ringbuf->Mask;
}

void UartBufClear(UartBuf *Ringbuf) {
	Ringbuf->Rd_Indx=Ringbuf->Wd_Indx;
}

void UartSendBuffer(uint8_t *dat, uint8_t len) {
	uint8_t i;
	for(i=0;i<len;i++){ //while (len--) {
		UartBuf_WD(&UartTxbuf, *dat); //UartBuf_WD(&UartTxbuf, *dat++);
		dat++;
	}
	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
}

void UartPutCh(uint8_t ch){
	UartBuf_WD(&UartTxbuf, ch);
	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
}
#endif


