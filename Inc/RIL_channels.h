/**
 * @file RIL_channels_PCF8574AN.h
 * @brief Заголовочный файл драйвера расширителя портов PCF8574AN
 */

#ifndef ROUT_INDICATOR_LIGHT_RIL_CHANNELS_H
#define ROUT_INDICATOR_LIGHT_RIL_CHANNELS_H

#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"

#define PCF8574AN_I2C hi2c1
#define PCF8574AN_QUANTITY 4 // Кол-во PCF8574AN
#define PCF8574AN_1_ADDRESS_WRITE 0x70 //Адрес 1-го PCF8574AN на запись
#define PCF8574AN_1_ADDRESS_READ 0x71 //Адрес 1-го PCF8574AN на чтение

extern I2C_HandleTypeDef hi2c1;

void channels_init (void);
void channels_read (uint8_t *pots);

#ifdef __cplusplus
}
#endif

#endif //ROUT_INDICATOR_LIGHT_RIL_CHANNELS_H
