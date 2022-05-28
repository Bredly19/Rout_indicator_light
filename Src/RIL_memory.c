#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "AT24Cxx.h"
#include "RIL_memory.h"

static bool mem_read_array_2char(PictureArray *pictureArray, const char *str) {
    uint8_t buffer[2][SIZE_ARR] = {0};
    uint16_t address = 0;

    if (AT24Cxx_IsConnected() == HAL_OK) {
        for (int i = 0; i < 2; ++i) {
            if (isdigit(*(str + i))) {
                address = ADDR_DIGITS + (*(str + i) - '0') * SIZE_ARR;
            } else {
                address = ADDR_LETTERS + (*(str + i) - 'A') * SIZE_ARR;
            }
            AT24Cxx_ReadEEPROM(address, buffer[i], SIZE_ARR);
        }
    } else return false;

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 16; ++j) {
            pictureArray->first_picture_arr[j + i * 16] = i % 2? buffer[1][j] : buffer[0][j+ i * 8] ;
            pictureArray->second_picture_arr[j + i * 16] = i % 2? buffer[0][j + 32] : buffer[1][j + 16 + i * 8] ;
        }
    }
    return true;
}

static bool mem_read_array_1char(PictureArray *pictureArray, const char *str) {
    uint16_t address = 0;

    if (isdigit(*str)) {
        address = ADDR_SINGLE_DIGITS + (*str - '0') * SIZE_ARR * 2;
    } else {
        address = ADDR_SINGLE_LETTERS + (*str - 'A') * SIZE_ARR * 2;
    }

    if (AT24Cxx_IsConnected() == HAL_OK) {
        AT24Cxx_ReadEEPROM(address, pictureArray->first_picture_arr, SIZE_ARR);
        AT24Cxx_ReadEEPROM(address + SIZE_ARR, pictureArray->second_picture_arr, SIZE_ARR);
    } else return false;

    return true;
}

static bool mem_read_array_spec_symb(PictureArray *pictureArray, const char *str) {
    char *spec_symbols[4] = {"arrow straight", "arrow left", "arrow right", "KGU"};

    if (AT24Cxx_IsConnected() == HAL_OK) {
        if ((memcmp(str, spec_symbols[0], 15) == 0) ) {
            AT24Cxx_ReadEEPROM(ADDR_SPEC_SYMBOLS, pictureArray->first_picture_arr, SIZE_ARR);
            AT24Cxx_ReadEEPROM(ADDR_SPEC_SYMBOLS + SIZE_ARR, pictureArray->second_picture_arr, SIZE_ARR);
        } else if ((memcmp(str, spec_symbols[1], 11) == 0)) {
            AT24Cxx_ReadEEPROM(ADDR_SPEC_SYMBOLS + SIZE_ARR * 2, pictureArray->first_picture_arr, SIZE_ARR);
            AT24Cxx_ReadEEPROM(ADDR_SPEC_SYMBOLS + SIZE_ARR * 2 + SIZE_ARR, pictureArray->second_picture_arr, SIZE_ARR);
        } else if (memcmp(str, spec_symbols[2], 12) == 0) {
            AT24Cxx_ReadEEPROM(ADDR_SPEC_SYMBOLS + SIZE_ARR * 4, pictureArray->first_picture_arr, SIZE_ARR);
            AT24Cxx_ReadEEPROM(ADDR_SPEC_SYMBOLS + SIZE_ARR * 4 + SIZE_ARR, pictureArray->second_picture_arr, SIZE_ARR);
        } else if (memcmp(str, spec_symbols[3], 4) == 0) {
            AT24Cxx_ReadEEPROM(ADDR_SPEC_SYMBOLS + SIZE_ARR * 6, pictureArray->first_picture_arr, SIZE_ARR);
            AT24Cxx_ReadEEPROM(ADDR_SPEC_SYMBOLS + SIZE_ARR * 6 + SIZE_ARR, pictureArray->second_picture_arr, SIZE_ARR);
        }
    } else return false;

    return true;
}

void mem_read_array(PictureArray *pictureArray, const char *str, size_t size) {
    switch (size) {
        case 1: {
            mem_read_array_1char(pictureArray, str);
            break;
        }
        case 2: {
            mem_read_array_2char(pictureArray, str);
        }
        default: {
            mem_read_array_spec_symb(pictureArray, str);
            break;
        }
    }

}

bool mem_read_array_1char_hor(PictureArray *pictureArray, const char *str) {
    uint16_t address = 0;

    if (isdigit(*str)) {
        address = ADDR_HORIZONTAL_DIGITS + (*str - '0') * SIZE_ARR * 2;
    } else {
        address = ADDR_HORIZONTAL_LETTERS + (*str - 'A') * SIZE_ARR * 2;
    }

    if (AT24Cxx_IsConnected() == HAL_OK) {
        AT24Cxx_ReadEEPROM(address, pictureArray->first_picture_arr, SIZE_ARR);
        AT24Cxx_ReadEEPROM(address + SIZE_ARR, pictureArray->second_picture_arr, SIZE_ARR);
    } else return false;

    return true;
}

bool mem_search_channel_settings(uint32_t *setting_number, uint8_t* sought, uint16_t size) {
    uint8_t end_ofSettings[size];
    uint8_t data[size];
    *setting_number = 0;

    memset(end_ofSettings, 0xff, size);
    memset(data, 0x00, size);

    do {
        AT24Cxx_ReadEEPROM(ADDR_CHANNEL_NUMBER + (*setting_number) * SIZE_CHANNEL_SETTINGS, data, sizeof(data));
        if (!memcmp(data, sought, size))
            return true;
        ++(*setting_number);
    } while (memcmp(data, end_ofSettings, size) != 0);

    --(*setting_number);

    return false;
}

void mem_read_combOrien_setting(ChannelSettings *channelSettings) {
    AT24Cxx_ReadEEPROM(ADDR_COMBINATION_NUMBER + channelSettings->setting_num * SIZE_CHANNEL_SETTINGS,
                       channelSettings->combination.ui8,
                       sizeof(channelSettings->combination.ui8));

    AT24Cxx_ReadEEPROM(ADDR_ORIENTATION + channelSettings->setting_num * SIZE_CHANNEL_SETTINGS,
                       &(channelSettings->orientation),
                       sizeof(channelSettings->orientation));
}

void mem_write_channel_settings(ChannelSettings *channelSettings) {
    AT24Cxx_WriteEEPROM(ADDR_CHANNEL_NUMBER + channelSettings->setting_num * SIZE_CHANNEL_SETTINGS,
                        channelSettings->channels,
                        sizeof(channelSettings->channels));

    AT24Cxx_WriteEEPROM(ADDR_COMBINATION_NUMBER + channelSettings->setting_num * SIZE_CHANNEL_SETTINGS,
                        channelSettings->combination.ui8,
                        sizeof(channelSettings->channels));

    AT24Cxx_WriteEEPROM(ADDR_ORIENTATION + channelSettings->setting_num * SIZE_CHANNEL_SETTINGS,
                        &channelSettings->orientation,
                        sizeof(channelSettings->orientation));
}

uint32_t mem_delete_settings(uint32_t setting_number) {
    uint8_t end_ofSettings[SIZE_CHANNEL_SETTINGS];
    uint8_t data[SIZE_CHANNEL_SETTINGS];

    memset(end_ofSettings, 0xff, SIZE_CHANNEL_SETTINGS);

    do {
        AT24Cxx_ReadEEPROM(ADDR_CHANNEL_NUMBER + setting_number * SIZE_CHANNEL_SETTINGS + SIZE_CHANNEL_SETTINGS,
                           data, sizeof(data));
        AT24Cxx_WriteEEPROM(ADDR_CHANNEL_NUMBER + setting_number * SIZE_CHANNEL_SETTINGS, data, sizeof(data));
        ++setting_number;
    } while (memcmp(data, end_ofSettings, SIZE_CHANNEL_SETTINGS) != 0);
    return --setting_number;
}

void mem_read_brightness_settings(BrightnessSettings *brightnessSettings) {
    uint8_t buffer = 0;

    AT24Cxx_ReadEEPROM(ADDR_DAYTIME_BRIGHTNESS, &buffer, sizeof(buffer));
    brightnessSettings->daytime_brightness = buffer >= 15? 15 : buffer < 12? 12 : buffer;

    AT24Cxx_ReadEEPROM(ADDR_CLOUDY_BRIGHTNESS, &buffer, sizeof(buffer));
    brightnessSettings->cloudy_brightness = buffer >= 11? 11 : buffer < 8? 8 : buffer;

    AT24Cxx_ReadEEPROM(ADDR_MIST_BRIGHTNESS, &buffer, sizeof(buffer));
    brightnessSettings->mist_brightness = buffer >= 7? 7 : buffer < 4? 4 : buffer;

    AT24Cxx_ReadEEPROM(ADDR_NIGHT_BRIGHTNESS, &buffer, sizeof(buffer));
    brightnessSettings->night_brightness = buffer >= 3? 3 : buffer <= 0? 0 : buffer;
}

void mem_write_brightness_settings(BrightnessSettings *brightnessSettings) {
    AT24Cxx_WriteEEPROM(ADDR_DAYTIME_BRIGHTNESS, &brightnessSettings->daytime_brightness, 1);
    AT24Cxx_WriteEEPROM(ADDR_CLOUDY_BRIGHTNESS, &brightnessSettings->cloudy_brightness, 1);
    AT24Cxx_WriteEEPROM(ADDR_MIST_BRIGHTNESS, &brightnessSettings->mist_brightness, 1);
    AT24Cxx_WriteEEPROM(ADDR_NIGHT_BRIGHTNESS, &brightnessSettings->night_brightness, 1);
}



