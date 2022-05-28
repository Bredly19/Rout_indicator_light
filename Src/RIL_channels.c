/**
 * @file RIL_channels_PCF8574AN.с
 * @brief Этот файл является драйвером расширителя портов PCF8574AN и
 *          предоставляет функции инициализации и считывания активных pin-ов.
 */

#include "RIL_channels.h"

/**
 * @brief Функция инициализации для четырёх расширителей портов PCF5874A.
 */
void channels_init(void) {
        uint8_t data = 0;

    for (int i = 0; i < PCF8574AN_QUANTITY * 2; i += 2) {
        //Проверяем готова ли PCF8574AN_х
        if (HAL_I2C_IsDeviceReady(&PCF8574AN_I2C,
                                  PCF8574AN_1_ADDRESS_WRITE + i, //Адрес 0х70, 0х72, 0х74, 0х76
                                  1 ,
                                  100) == HAL_OK) {
            //Обнуляем входы/выходы PCF8574AN_х
            while (HAL_I2C_Master_Transmit(&PCF8574AN_I2C,
                                           PCF8574AN_1_ADDRESS_WRITE + i, //Адрес 0х70, 0х72, 0х74, 0х76
                                           &data,
                                           sizeof(data),
                                           100) != HAL_OK);
        }
    }
}

/**
 * @brief Функция считывания активных pin-ов в каналах
 * @param channels Указатель на массив в котором будут храниться активные pin-ы каналов
 */

#define NUMBER_OF_CHANNELS 2

void channels_read(uint8_t *channels) {
    uint8_t data[4]  = {0};
    uint16_t buffer[NUMBER_OF_CHANNELS] = {0};
    uint8_t j = 0;

    for (int i = 0; i < PCF8574AN_QUANTITY * 2; i += 2, ++j) {
        //Проверяем готова ли PCF8574AN_х
        if (HAL_I2C_IsDeviceReady(&PCF8574AN_I2C,
                                  PCF8574AN_1_ADDRESS_READ + i, //Адрес 0х71, 0х73, 0х75, 0х77
                                  1,
                                  100) == HAL_OK) {
            //Считываем значение с PCF8574AN_х
            while (HAL_I2C_Master_Receive(&PCF8574AN_I2C,
                                          PCF8574AN_1_ADDRESS_READ + i, //Адрес 0х71, 0х73, 0х75, 0х77
                                          &data[j],
                                          sizeof(data[j]),
                                          100) != HAL_OK);
        } else {
            i -= 2; //Если PCF8574AN_х не готова, проверяем еще раз
            --j;
        }
    }

    /*Преобразуем значение полученное с PCF8574AN_х в номер активного pin-а*/
    buffer[0] = data[0] + data[1] * 256;
    buffer[1] = data[2] + data[3] * 256;

    for (int i = 0; i < NUMBER_OF_CHANNELS; ++i) {
        switch (buffer[i]) {
            case 0:
                channels[i] = 0;
                break;
            case 1:
                channels[i] = 1;
                break;
            case 2:
                channels[i] = 2;
                break;
            case 4:
                channels[i] = 3;
                break;
            case 8:
                channels[i] = 4;
                break;
            case 16:
                channels[i] = 5;
                break;
            case 32:
                channels[i] = 6;
                break;
            case 64:
                channels[i] = 7;
                break;
            case 128:
                channels[i] = 8;
                break;
            case 256:
                channels[i] = 9;
                break;
            case 512:
                channels[i] = 10;
                break;
            case 1024:
                channels[i] = 11;
                break;
            default:
                channels[i] = 0xff;
        }
    }
}
