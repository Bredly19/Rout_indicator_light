#ifndef ROUT_INDICATOR_LIGHT_AT24CXX_H
#define ROUT_INDICATOR_LIGHT_AT24CXX_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "stdbool.h"

#define	EEPROM_I2C			hi2c1
#define EEPROM_ADDRESS		0x50
#define EEPROM_PAGESIZE		128
#define EEPROM_WRITE		10					//time to wait in ms
#define EEPROM_TIMEOUT		(5 * EEPROM_WRITE)	//timeout while writing

extern I2C_HandleTypeDef EEPROM_I2C;

HAL_StatusTypeDef AT24Cxx_IsConnected(void);
HAL_StatusTypeDef AT24Cxx_ReadEEPROM(unsigned address, const void* src, unsigned len);
HAL_StatusTypeDef AT24Cxx_WriteEEPROM(unsigned address, const void* src, unsigned len);

#ifdef __cplusplus
}
#endif

#endif //ROUT_INDICATOR_LIGHT_AT24CXX_H
