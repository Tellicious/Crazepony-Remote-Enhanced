#ifndef __CONFIG_TABLE_H
#define __CONFIG_TABLE_H
//
#include "stm32f10x.h"

typedef struct config_table_tt {
uint16_t version;
uint16_t thrCalibraValue;
uint16_t pitchCalibraValue;
uint16_t rollCalibraValue;
uint16_t yawCalibraValue;
uint16_t tx_addr;
}config_table_t;

extern config_table_t table;	
extern uint8_t gParamsSaveEEPROMRequest;

void LoadParamsFromEEPROM(void);
void ParamSetDefault(void) ;
void ParamToTable(void);
void TableToParam(void);
void TableWriteEEPROM(void);
void TableReadEEPROM(void);
void SaveParamsToEEPROM(void);

#endif
