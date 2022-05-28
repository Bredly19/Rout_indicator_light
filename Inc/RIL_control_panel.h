#ifndef ROUT_INDICATOR_LIGHT_RIL_CONTROL_PANEL_H
#define ROUT_INDICATOR_LIGHT_RIL_CONTROL_PANEL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdbool.h>

#define CP_HANDLE_SPI           hspi2
#define CP_HANDLE_TIM           htim1
#define CHARTERS_OUT_STR        4
#define KEY_HOLD_TIME           1000

extern SPI_HandleTypeDef hspi2;
extern TIM_HandleTypeDef htim1;

typedef enum {
    REST_TICK = 0,
    OFF,
    ON
}SegmentOnOff_t;

typedef enum {
    INACTIVE = 0,
    ACTIVE
}SegmentState;

typedef struct {
    char out_str[5];
    int8_t num_segment_blink;
    bool all_segments_blink;
}DigitIndicator;

typedef struct {
    GPIO_PinState pinState;
    uint32_t start_tick;

    enum {
        BUTTON_1 = 1,
        BUTTON_2,
        BUTTON_3
    } KeyNumber;

    enum {
        WAITING = 0,
        FRONT_BOUNCE,
        PRESSING,
        REAR_BOUNCE
    } KeySignalState;

    enum {
        NOT_PRESSED = 0,
        PRESSED,
        LONG_PRESS,
        SHORT_PRESS,
    } KeyPressedState;

} Button_t;


uint16_t str_to_uint (char* str);

void digit_indicator_init(void);
void digit_indicator_start(void);
void digit_indicator_stop(void);
void digit_indicator_print(char *format, const uint16_t *num);
void digit_indicator_print_ports(uint8_t *ports);
void digit_indicator_blink_all(void);
void digit_indicator_not_blink_all(void);
void digit_indicator_blink_next(void);
void digit_indicator_increment(uint16_t begin_value, uint16_t end_value);
void digit_indicator_not_blink(void);
char* digit_indicator_get_string(void);

void buttons_init(void);
bool button_long_press(Button_t* key);
bool button_short_press(Button_t* key);

void led_green_on(void);
void led_red_on(void);
void led_yellow_on(void);

#ifdef __cplusplus
}
#endif

#endif //ROUT_INDICATOR_LIGHT_RIL_CONTROL_PANEL_H
