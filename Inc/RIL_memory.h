#ifndef ROUT_INDICATOR_LIGHT_RIL_MEMORY_H
#define ROUT_INDICATOR_LIGHT_RIL_MEMORY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#define DIGITS                  (10) //Кол-во цифр (0 - 9)
#define LETTERS                 (24) //Кол-во букв
#define SPEC_SYMBOLS            (4 * 2) //стрелка прямо, стрелка в лево, стрелка в право, КГУ
#define SIZE_ARR                (uint32_t)(48) //размер массива
#define ADDR_DIGITS             (uint32_t)(0x00) // Адрес цифр в памяти
#define ADDR_LETTERS            (uint32_t)(ADDR_DIGITS + (SIZE_ARR * DIGITS))

#define ADDR_SPEC_SYMBOLS       (uint32_t)(ADDR_LETTERS + (SIZE_ARR * LETTERS))

#define ADDR_SINGLE_DIGITS      (uint32_t)(ADDR_SPEC_SYMBOLS + (SIZE_ARR * SPEC_SYMBOLS))
#define ADDR_SINGLE_LETTERS     (uint32_t)(ADDR_SINGLE_DIGITS + (SIZE_ARR * DIGITS * 2))

#define ADDR_HORIZONTAL_DIGITS  (uint32_t)(ADDR_SINGLE_LETTERS + (SIZE_ARR * LETTERS * 2))
#define ADDR_HORIZONTAL_LETTERS (uint32_t)(ADDR_HORIZONTAL_DIGITS + (SIZE_ARR * DIGITS * 2))

#define ADDR_DAYTIME_BRIGHTNESS (uint32_t)(0x9B90)
#define ADDR_CLOUDY_BRIGHTNESS  (uint32_t)(ADDR_DAYTIME_BRIGHTNESS + sizeof(uint8_t))
#define ADDR_MIST_BRIGHTNESS    (uint32_t)(ADDR_CLOUDY_BRIGHTNESS + sizeof(uint8_t))
#define ADDR_NIGHT_BRIGHTNESS   (uint32_t)(ADDR_MIST_BRIGHTNESS + sizeof(uint8_t))

#define SIZE_CHANNEL_SETTINGS   5
#define ADDR_CHANNEL_NUMBER     (uint32_t)(0x9BA0)
#define ADDR_COMBINATION_NUMBER (uint32_t)(ADDR_CHANNEL_NUMBER + sizeof(uint16_t))
#define ADDR_ORIENTATION        (uint32_t)(ADDR_COMBINATION_NUMBER + sizeof(uint16_t))

#define SIZE_CHANNEL            sizeof(channelSettings.channels)
#define SIZE_COMBINATION_NUMBER sizeof(channelSettings.combination.ui8)
#define SIZE_ORIENTATION        sizeof(channelSettings.orientation)

#define HORIZONTALLY 1
#define VERTICAL   0

typedef struct {
    uint8_t first_picture_arr[SIZE_ARR];
    uint8_t second_picture_arr[SIZE_ARR];
} PictureArray;

typedef struct {
    uint8_t orientation;
    uint8_t channels[2];
    union {
        uint16_t ui16;
        uint8_t ui8[2];
    } combination;
    uint32_t setting_num;
} ChannelSettings;

typedef struct {
    uint8_t daytime_brightness;
    uint8_t cloudy_brightness;
    uint8_t mist_brightness;
    uint8_t night_brightness;
} BrightnessSettings ;

void mem_read_array(PictureArray *pictureArray, const char *str, size_t size);
bool mem_read_array_1char_hor(PictureArray *pictureArray, const char *str);
bool mem_search_channel_settings(uint32_t *setting_number, uint8_t* sought, uint16_t size);
void mem_read_combOrien_setting(ChannelSettings *channelSettings);
void mem_write_channel_settings(ChannelSettings *channelSettings);
uint32_t mem_delete_settings(uint32_t setting_number);
void mem_read_brightness_settings(BrightnessSettings *brightnessSettings);
void mem_write_brightness_settings(BrightnessSettings *brightnessSettings);

#ifdef __cplusplus
}
#endif

#endif //ROUT_INDICATOR_LIGHT_RIL_MEMORY_H
