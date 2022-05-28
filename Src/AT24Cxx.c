#include "AT24Cxx.h"

// Превращаем адрес с 7 бит в 8 бит
static unsigned eeprom_address = EEPROM_ADDRESS << 1;
// Маску вычисляем по заданному размеру страницы
static unsigned inpage_addr_mask = EEPROM_PAGESIZE - 1;

static HAL_StatusTypeDef AT24Cxx_WriteReadEEPROM(unsigned address, const void* src, unsigned len, bool write);
static unsigned size_to_page_end(unsigned addr);

/**
 * @brief Проверяет, готова ли EEPROM AT24Cxx к связи.
 * @return Возвращает поле перечисления HAL_StatusTypeDef (HAL_OK, HAL_ERROR, HAL_BUSY, HAL_TIMEOUT)
 */
HAL_StatusTypeDef AT24Cxx_IsConnected(void) {
    return HAL_I2C_IsDeviceReady(&EEPROM_I2C, eeprom_address, 1, EEPROM_TIMEOUT);
}

/**
 * @brief Функция считывания данных с EEPROM AT24Cxx
 * @param address Адрес EEPROM с которого читаем
 * @param src Указатель на память в которую пишем то что считали с EEPROM
 * @param len Размер памяти в которую пишем
 * @return Возвращает поле перечисления HAL_StatusTypeDef (HAL_OK, HAL_ERROR, HAL_BUSY, HAL_TIMEOUT)
 */
HAL_StatusTypeDef AT24Cxx_ReadEEPROM(unsigned address, const void* src, unsigned len) {
    return AT24Cxx_WriteReadEEPROM(address, src, len, false);
}

/**
 * @brief Функция записи данных в EEPROM AT24Cxx
 * @param address Адрес EEPROM с которого пишем
 * @param src Указатель на память с которой считываем данные для последующей записи в EEPROM
 * @param len Размер памяти с которой читаем
 * @return Возвращает поле перечисления HAL_StatusTypeDef (HAL_OK, HAL_ERROR, HAL_BUSY, HAL_TIMEOUT)
 */
HAL_StatusTypeDef AT24Cxx_WriteEEPROM(unsigned address, const void* src, unsigned len) {
    return AT24Cxx_WriteReadEEPROM(address, src, len, true);
}

static HAL_StatusTypeDef AT24Cxx_WriteReadEEPROM(unsigned address, const void* src, unsigned len, bool write) {
    uint8_t *pdata = (uint8_t*) src;

    HAL_StatusTypeDef result = HAL_OK;

    // Первая порция не может быть больше, чтобы не пересекла размер страницы
    unsigned max_portion = size_to_page_end(address);
    unsigned portion;

    while (len != 0 && result == HAL_OK) {
        portion = len;              // Все, что осталось - в одну порцию

        if (portion > max_portion) {
            portion = max_portion;  // Порция великовата, уменьшаем
        }

        // Работаем с порцией, которую можно проглотить
        if(write) {
            result = HAL_I2C_Mem_Write(&EEPROM_I2C,
                                       eeprom_address,
                                       address,
                                       I2C_MEMADD_SIZE_16BIT,
                                       pdata,
                                       portion,
                                       EEPROM_TIMEOUT);
        }
        else {
            result = HAL_I2C_Mem_Read(&EEPROM_I2C,
                                      eeprom_address,
                                      address,
                                      I2C_MEMADD_SIZE_16BIT,
                                      pdata,
                                      portion,
                                      EEPROM_TIMEOUT);
        }

        // И регистрируем, что уже это сделали
        len     -= portion;
        address += portion;
        pdata   += portion;

        // следующее, если вообще будет, будет изначально страницы
        max_portion = EEPROM_PAGESIZE;

        if(write) {
            HAL_Delay(EEPROM_WRITE);
        }
        else {
            HAL_Delay(EEPROM_WRITE / 2);
        }
    }

    return result;
}

// вычисления расстояния до конца страницы
static unsigned size_to_page_end(unsigned addr) {
    return (~addr & inpage_addr_mask) + 1;
}
