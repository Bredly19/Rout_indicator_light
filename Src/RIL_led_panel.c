#include "RIL_led_panel.h"

//#define INVERT_BYTE(a) ((((a)&1)<<7) | (((a)&2)<<5) | (((a)&4)<<3) | (((a)&8)<<1) | (((a)&16)>>1) | (((a)&32)>>3) | (((a)&64)>>5) | (((a)&128)>>7))

CSPin CS_pin_line[NUM_OF_LINES_LED_PANEL];

uint32_t start_tick = 0;
TimesOfDay times_of_day, past_times_of_day = LD;
StateDelay state_delay = ZERO;
volatile uint16_t adc;

void max7219_send(uint8_t reg, uint8_t data){
    uint8_t package[2]={reg,data};
    while((HAL_SPI_Transmit(&LP_HANDLE_SPI, package, sizeof(package), 100) != HAL_OK));
}

void max7219_send_to_all(uint8_t reg, uint8_t data){
    for (int i = 0; i < NUM_OF_LINES_LED_PANEL; ++i) {
        HAL_GPIO_WritePin(CS_pin_line[i].Port, CS_pin_line[i].Pin, GPIO_PIN_RESET);
        for (int8_t j = 0; j < NUM_OF_LINES_MAX7219; ++j) {
            max7219_send(reg, data);
        }
        HAL_GPIO_WritePin(CS_pin_line[i].Port, CS_pin_line[i].Pin, GPIO_PIN_SET);
    }
}

void max7219_clean_all(void) {
    for (int j = 0; j < NUM_OF_LINES_MAX7219; j++) {
        max7219_send_to_all(REG_DIGIT_0 + j, 0x00);
    }
}

void led_panel_init(void) {
    CS_pin_line[0].Port = CS_LINE1_GPIO_Port;
    CS_pin_line[0].Pin = CS_LINE1_Pin;
    CS_pin_line[1].Port = CS_LINE2_GPIO_Port;
    CS_pin_line[1].Pin = CS_LINE2_Pin;
    CS_pin_line[2].Port = CS_LINE3_GPIO_Port;
    CS_pin_line[2].Pin = CS_LINE3_Pin;
    HAL_Delay(50);

    /*Подготовка Led панели*/
    max7219_send_to_all(REG_SHUTDOWN,0x01);         //  power down = 0,normal mode = 1
    max7219_send_to_all(REG_DECODE_MODE, 0x00);     //  no decoding
    max7219_send_to_all(REG_INTENSITY, 0x05);       //  brightness intensity
    max7219_send_to_all(REG_SCAN_LIMIT, 0x07);      //  scan limit = 8 LEDs
    max7219_send_to_all(REG_DISPLAY_TEST, 0x00);    //  no test matrix

    /*Настройка ADC*/
    HAL_ADCEx_Calibration_Start(&LP_HANDLE_ADC);

    max7219_clean_all();
}

void led_panel_clean(void) {
    max7219_clean_all();
}

void led_panel_test_on(void) {
    max7219_send_to_all(REG_DISPLAY_TEST, 0x01);
}

void led_panel_test_off(void) {
    max7219_send_to_all(REG_DISPLAY_TEST, 0x00);
}

void led_panel_print(void* data){
    uint8_t* buffer = (uint8_t*)data;
    for (int i = 0; i < NUM_OF_LINES_LED_PANEL; ++i) {
        for (int j = 0; j < NUM_OF_LINES_MAX7219; ++j) {
            HAL_GPIO_WritePin(CS_pin_line[i].Port, CS_pin_line[i].Pin, GPIO_PIN_RESET);
            for (int k = 0; k < 25; k += 8) {
                max7219_send(REG_DIGIT_7 - j, buffer[i * 32 + j + k]);
            }
            HAL_GPIO_WritePin(CS_pin_line[i].Port, CS_pin_line[i].Pin, GPIO_PIN_SET);
        }
    }
}


/*       Функции отвечающие за настройку яркости Led панели взависимости от времени суток        */

static void set_times_of_day(void) {
    if (adc < ADC_VALUE_DAYTIME) {
        times_of_day = LD;
    }
    else if (adc < ADC_VALUE_CLOUDY) {
        times_of_day = LG;
    }
    else if (adc < ADC_VALUE_MIST) {
        times_of_day = LT;
    }
    else
        times_of_day = LN;
}

void led_panel_change_brightness(void *brightness) {
    uint8_t *pbr = (uint8_t*)brightness;
    HAL_ADCEx_InjectedStart(&LP_HANDLE_ADC); // запускаем опрос инжект. каналов
    HAL_ADC_PollForConversion(&LP_HANDLE_ADC,100); // ждём окончания

    adc = HAL_ADCEx_InjectedGetValue(&LP_HANDLE_ADC, ADC_INJECTED_RANK_1); //результат опроса канала (ADC) копируем в переменную
    set_times_of_day();
    max7219_send_to_all(REG_INTENSITY, pbr[times_of_day]);
}

bool led_panel_change_brightness_with_delay(void *brightness) {
    uint8_t *pbr = (uint8_t*)brightness;

    HAL_ADCEx_InjectedStart(&LP_HANDLE_ADC); // запускаем опрос инжект. каналов
    HAL_ADC_PollForConversion(&LP_HANDLE_ADC,100); // ждём окончания

    adc = HAL_ADCEx_InjectedGetValue(&LP_HANDLE_ADC, ADC_INJECTED_RANK_1); //результат опроса канала (ADC) копируем в переменную
    set_times_of_day();

    switch (state_delay) {
        case ZERO:
            if (past_times_of_day != times_of_day) {
                state_delay = ONE;
                start_tick = HAL_GetTick();
            }
            break;
        case ONE:
            if (HAL_GetTick() - start_tick > SWITCHING_DELAY_SEC(30)) {
                max7219_send_to_all(REG_INTENSITY, pbr[times_of_day]);
                state_delay = ZERO;
                past_times_of_day = times_of_day;
                return true;
            } else if (past_times_of_day == times_of_day) {
                state_delay = ZERO;
            }
            break;
        default:;
    }
    return false;
}

