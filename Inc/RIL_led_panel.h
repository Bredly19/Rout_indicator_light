#ifndef ROUT_INDICATOR_LIGHT_RIL_LED_PANEL_H
#define ROUT_INDICATOR_LIGHT_RIL_LED_PANEL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdbool.h>

/*Регистры Max7219*/
#define REG_NO_OP           0x00
#define REG_DIGIT_0         0x01
#define REG_DIGIT_1         0x02
#define REG_DIGIT_2         0x03
#define REG_DIGIT_3         0x04
#define REG_DIGIT_4         0x05
#define REG_DIGIT_5         0x06
#define REG_DIGIT_6         0x07
#define REG_DIGIT_7         0x08
#define REG_DECODE_MODE     0x09
#define REG_INTENSITY       0x0A
#define REG_SCAN_LIMIT      0x0B
#define REG_SHUTDOWN        0x0C
#define REG_DISPLAY_TEST    0x0F

#define NUM_OF_LINES_LED_PANEL  3 // Кол-во линий в Led панели
#define QTY_MAX7219             4 // Кол-во max7219 в линии
#define NUM_OF_LINES_MAX7219    8 // Кол-во строк max7219

#define SWITCHING_DELAY_SEC(sec) ((sec) * 1000)

#define LP_HANDLE_SPI       hspi1
#define LP_HANDLE_ADC       hadc1

#define ADC_VALUE_DAYTIME   1000
#define ADC_VALUE_CLOUDY    2000
#define ADC_VALUE_MIST      3000


extern SPI_HandleTypeDef hspi1;
extern ADC_HandleTypeDef hadc1;

typedef struct {
    GPIO_TypeDef* Port;
    uint16_t Pin;
} CSPin;

typedef enum {
    LD = 0,
    LG,
    LT,
    LN
} TimesOfDay;

typedef enum {
    ZERO = 0,
    ONE
} StateDelay;

void led_panel_init(void);
void led_panel_clean(void);
void led_panel_print(void* data);
void led_panel_test_on(void);
void led_panel_test_off(void);

bool led_panel_change_brightness_with_delay(void *brightness);
void led_panel_change_brightness(void *brightness);

#ifdef __cplusplus
}
#endif

#endif //ROUT_INDICATOR_LIGHT_RIL_LED_PANEL_H
