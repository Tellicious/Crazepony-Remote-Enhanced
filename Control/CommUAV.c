

#include "CommUAV.h"
#include <stdio.h>
#include "stdint.h"
#include "NRF24.h"
#include "control.h"
#include "Config_Params.h"

uint8_t sendBuf[32];
uint8_t sendCnt=0;

static  void uart8chk(uint8_t _x) {
  sendBuf[sendCnt++] = _x;     
}

static void uart16chk(int16_t a){
    uart8chk((uint8_t)(a & 0xff));
    uart8chk((uint8_t)(a >> 8));
}

uint8_t CommUAVUpload(uint8_t cmd) {    
  sendCnt=0;
    
  uart8chk('$');
  uart8chk('M');
  uart8chk('<');

	uart8chk(8);
	uart8chk(cmd);
  		
	uart16chk(Throttle);
	uart16chk(Yaw);
	uart16chk(Pitch);
	uart16chk(Roll);
	return NRF24_safeWrite(sendBuf, sizeof(sendBuf), 1, 1, 8);
}
