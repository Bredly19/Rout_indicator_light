#include <string.h>
#include <ctype.h>
#include "RIL_control_panel.h"

//#define IS_DIGIT(arg) ((arg >= '0') && (arg <= '9'))

/* байты чисел от 0 до 9 */
uint8_t segment_map_digit[] = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0X80,0X90};
/* байты букв от a до z */
uint8_t segment_map_alpha[] = {136, 131, 167, 161, 134, 142, 144, 139 ,207, 241, 182, 199, 182, 171, 163, 140, 152, 175, 146, 135, 227, 182, 182, 182, 145, 182};
/* байт-карты для выбора разряда дисплея от 1 до 4 */
uint8_t segment_select[] = {0xF1,0xF2,0xF4,0xF8};

static DigitIndicator digitIndicator;

static void digit_indicator_clean(void) {
    uint8_t empty = 0xff;
    for (int i = 0; i < CHARTERS_OUT_STR; ++i) {
        HAL_GPIO_WritePin (CS_DigitIndicator_GPIO_Port, CS_DigitIndicator_Pin, GPIO_PIN_RESET);
        HAL_SPI_Transmit(&CP_HANDLE_SPI, &empty, 1, 100);
        HAL_SPI_Transmit(&CP_HANDLE_SPI, &segment_select[i], 1, 100);
        HAL_GPIO_WritePin (CS_DigitIndicator_GPIO_Port, CS_DigitIndicator_Pin, GPIO_PIN_SET);
    }
}

static uint8_t convert_num_to_str(char *str, uint16_t num) {
    char *ptr_buffer;
    uint16_t num_cpy = num;
    uint8_t digits = 0;

    do {
        num_cpy /=10;
        ++digits;
    } while (num_cpy);

    if (digits > CHARTERS_OUT_STR) {
        num /= (digits - CHARTERS_OUT_STR) * 10;
        ptr_buffer = str + CHARTERS_OUT_STR;
    } else ptr_buffer = str + digits;

    do {
        *(--ptr_buffer) = (char)(num % 10 + '0');
        num /= 10;
    } while (num);

    return digits;
}

void digit_indicator_print(char *format, const uint16_t *num) {
    char *pout_str = digitIndicator.out_str;
    char *travers = format;
    char buffer[5] = {0};
    uint8_t charters = 0, digits;

    memcpy(pout_str, "____", 5);

    while (charters < CHARTERS_OUT_STR) {
        if (*travers != '%' && *travers != '\0')
            *(pout_str + charters) = *travers++;
        else if (num != NULL && *(travers + 1) == 'd') {
            travers += 2;
            digits = convert_num_to_str(buffer, *num);
            digits = digits < (CHARTERS_OUT_STR - charters)? digits : CHARTERS_OUT_STR - charters;
            memcpy(pout_str + charters, buffer, digits);
            charters += digits - 1;
        }
        ++charters;
    }
}

void digit_indicator_print_ports (uint8_t *ports) {
    char *pout_str = digitIndicator.out_str;
    uint8_t *pports = ports;

    pout_str += CHARTERS_OUT_STR;

    for (int8_t i = 1; i >= 0; --i) {
        if (pports[i] < 10) {
            *(--pout_str) = pports[i] + '0';
            *(--pout_str) = '0';
        }else {
            do {
                *(--pout_str) = (char)(pports[i] % 10 + '0');
                pports[i] /= 10;
            } while (pports[i]);
        }
    }
}

void digit_indicator_init(void) {
    strcpy(digitIndicator.out_str, "____");
    digitIndicator.num_segment_blink = -1;
    digitIndicator.all_segments_blink = false;
    digit_indicator_clean();
}

void digit_indicator_start(void) {
    digit_indicator_clean();
    HAL_TIM_Base_Start_IT(&CP_HANDLE_TIM);
}

void digit_indicator_stop(void) {
    digit_indicator_clean();
    HAL_TIM_Base_Stop_IT(&CP_HANDLE_TIM);
}

static SegmentState blink(void) {
    SegmentState segmentState = ACTIVE;
    static uint32_t start_tick = 0;
    static SegmentOnOff_t onOff = OFF;
    static bool status_onoff = false;

    switch (onOff) {
        case REST_TICK: {
            start_tick = HAL_GetTick();
            if (!status_onoff) {
                onOff = OFF;
            } else {
                onOff = ON;
            }
            break;
        }
        case OFF: {
            segmentState = INACTIVE;
            if ((HAL_GetTick() - start_tick) > 500) {
                onOff = REST_TICK;
                status_onoff = true;
            }
            break;
        }
        case ON: {
            if ((HAL_GetTick() - start_tick) > 500) {
                onOff = REST_TICK;
                status_onoff = false;
            }
            break;
        }
        default:;
    }
    return segmentState;
}

static SegmentState digit_blink(uint8_t seg) {
    if (seg == digitIndicator.num_segment_blink) {
        return blink();
    }
    return ACTIVE;
}

static SegmentState digit_indicator_blink(void) {
    if (digitIndicator.all_segments_blink == true) {
        return blink();
    }
    return ACTIVE;
}

static void write_str_to_segment(DigitIndicator* display) {
    uint8_t empty = 0xff;
    static uint8_t segment = 0;

    if (digit_blink(segment) == INACTIVE) {
        if (++segment > 3)
            segment = 0;
    }

    if (digit_indicator_blink() == ACTIVE) {
        HAL_GPIO_WritePin (CS_DigitIndicator_GPIO_Port, CS_DigitIndicator_Pin, GPIO_PIN_RESET);
        if (display->out_str[segment] != '_') {
            if (isdigit(display->out_str[segment]))
                HAL_SPI_Transmit(&CP_HANDLE_SPI, &segment_map_digit[display->out_str[segment] - '0'], 1, 100);
            else
                HAL_SPI_Transmit(&CP_HANDLE_SPI, &segment_map_alpha[display->out_str[segment] - 'a'], 1, 100);
            HAL_SPI_Transmit(&CP_HANDLE_SPI, &segment_select[segment], 1, 100);
        }
        else {
            HAL_SPI_Transmit(&CP_HANDLE_SPI, &empty, 1, 100);
            HAL_SPI_Transmit(&CP_HANDLE_SPI, &segment_select[segment], 1, 100);
        }
        HAL_GPIO_WritePin (CS_DigitIndicator_GPIO_Port, CS_DigitIndicator_Pin, GPIO_PIN_SET);
    } else digit_indicator_clean();

    ++segment;
    if (segment > 3)
        segment = 0;
}

void digit_indicator_blink_all(void) {
    digitIndicator.num_segment_blink = -1;
    digitIndicator.all_segments_blink = true;
}

void digit_indicator_not_blink_all(void) {
    digitIndicator.all_segments_blink = false;
}

void digit_indicator_not_blink(void) {
    digitIndicator.num_segment_blink = -1;
}

void digit_indicator_blink_next(void) {
    digitIndicator.all_segments_blink = false;
    char *pout_str = digitIndicator.out_str;
    uint8_t digits = 0;

    while (isdigit(*(pout_str + digits))) {
        ++digits;
    }

    if (digitIndicator.num_segment_blink < digits - 1)
        ++digitIndicator.num_segment_blink;
    else digitIndicator.num_segment_blink = 0;
}

uint16_t str_to_uint(char* str) {
    uint16_t num = 0;
    for ( ;*str == '0'; ++str);
    for ( ;*str != '_'; ++str) {
        num = num + *str - '0';
        if (*(str + 1) != '_') {
            num *= 10;
        }
    }
    return num;
}

void digit_indicator_increment(uint16_t begin_value, uint16_t end_value) {
    if (digitIndicator.num_segment_blink >= 0) {
        if (digitIndicator.out_str[digitIndicator.num_segment_blink] >= '9')
            digitIndicator.out_str[digitIndicator.num_segment_blink] = '0';
        else ++digitIndicator.out_str[digitIndicator.num_segment_blink];

        while (begin_value > str_to_uint(digitIndicator.out_str))
            ++digitIndicator.out_str[digitIndicator.num_segment_blink];

        uint16_t num = str_to_uint(digitIndicator.out_str);
        if (end_value < num)
            digitIndicator.out_str[digitIndicator.num_segment_blink] = '0';
    }
}

char* digit_indicator_get_string(void) {
    return digitIndicator.out_str;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM1) {
        write_str_to_segment(&digitIndicator);
    }
}

/*----------------------------BUTTONS-------------------------------*/

Button_t button_1, button_2, button_3;

void buttons_init(void) {
    button_1.KeyNumber = BUTTON_1;
    button_2.KeyNumber = BUTTON_2;
    button_3.KeyNumber = BUTTON_3;
    button_1.KeySignalState = WAITING;
    button_2.KeySignalState = WAITING;
    button_3.KeySignalState = WAITING;
    button_1.KeyPressedState = NOT_PRESSED;
    button_2.KeyPressedState = NOT_PRESSED;
    button_3.KeyPressedState = NOT_PRESSED;
    button_1.start_tick = 0;
    button_2.start_tick = 0;
    button_3.start_tick = 0;
}

static uint8_t key_press_handler(Button_t* button) {
    switch (button->KeyNumber) {
        case BUTTON_1: {
            button->pinState = !HAL_GPIO_ReadPin(BUTTON1_GPIO_Port, BUTTON1_Pin);
            break;
        }
        case BUTTON_2: {
            button->pinState = !HAL_GPIO_ReadPin(BUTTON2_GPIO_Port, BUTTON2_Pin);
            break;
        }
        case BUTTON_3: {
            button->pinState = !HAL_GPIO_ReadPin(BUTTON3_GPIO_Port, BUTTON3_Pin);
            break;
        }
        default:;
    }

    switch (button->KeySignalState) {
            case WAITING: {
                if (button->pinState == GPIO_PIN_SET) {
                    button->KeySignalState = FRONT_BOUNCE;
                    button->KeyPressedState = PRESSED;
                    button->start_tick = HAL_GetTick();
                }
                break;
            }
            case FRONT_BOUNCE: {
                if ((HAL_GetTick() - button->start_tick) > 100) {
                    button->KeySignalState = PRESSING;
                    button->start_tick = HAL_GetTick();
                }
                break;
            }
            case PRESSING: {
                if (button->pinState == GPIO_PIN_SET) {
                    if ((HAL_GetTick() - button->start_tick) > KEY_HOLD_TIME - 100) {
                        button->KeyPressedState = LONG_PRESS;
                    }
                } else {
                    button->KeyPressedState = SHORT_PRESS;
                }
                break;
            }
            case REAR_BOUNCE: {
                if (button->pinState == GPIO_PIN_RESET) {
                    if ((HAL_GetTick() - button->start_tick) > 100) {
                        button->KeySignalState = WAITING;
                    }
                }
                break;
            }
            default:;
        }
    return button->KeyPressedState;
}

static void key_reset(Button_t* key) {
    key->KeySignalState = REAR_BOUNCE;
    key->KeyPressedState = NOT_PRESSED;
    key->start_tick = HAL_GetTick();
    key_press_handler(key);
}

bool button_long_press(Button_t* key) {
    if (key_press_handler(key) == LONG_PRESS) {
        key_reset(key);
        return true;
    }
    button_short_press(key);
    return false;
}

bool button_short_press(Button_t* key) {
    if (key_press_handler(key) == SHORT_PRESS) {
        key_reset(key);
        return true;
    }
    return false;
}

void led_green_on(void) {
    HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED_YELLOW_GPIO_Port, LED_YELLOW_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_SET);
}

void led_red_on(void) {
    HAL_GPIO_WritePin(LED_YELLOW_GPIO_Port, LED_YELLOW_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET);
};

void led_yellow_on(void) {
    HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED_YELLOW_GPIO_Port, LED_YELLOW_Pin, GPIO_PIN_SET);
}