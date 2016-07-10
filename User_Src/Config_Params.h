#ifndef _CFG_PARAMS_H_
#define _CFG_PARAMS_H_
#include <stdint.h>

// Clock defines
//#define RCC_CLOCK_HSE
#define RCC_CLOCK_HSI

// UART1 defines
#define UART1_BAUDRATE 115200
#define UART_DEBUG
//#define UART1_USE_INTERRUPTS
#define UART1_NO_INTERRUPTS
#define UART1_STDIO_FUN

// TIM period (TIM3 freq: 1kHz, TIM4 freq: !MHz)
#define TIMER3_Period		1000
#define TIMER4_Period  	500		


// LED defines
#define signalLED GPIOA, GPIO_Pin_12
#define led2      GPIOB, GPIO_Pin_4
#define led3      GPIOB, GPIO_Pin_5
#define led4      GPIOB, GPIO_Pin_6
#define led5      GPIOB, GPIO_Pin_7
#define NOT_signalLED !(GPIOA->ODR & GPIO_Pin_12)
#define NOT_led4 !(GPIOB->ODR & GPIO_Pin_6)




// Timer Flags
extern uint16_t flag10Hz,flag50Hz,flag80Hz,flag100Hz;

// NRF24 Address
extern uint8_t NRF24_tx_address[5];

// Command Values
extern uint16_t Throttle, Roll, Pitch, Yaw;

#endif
