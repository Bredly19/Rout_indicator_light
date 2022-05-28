#include <string.h>
#include "RIL_menu.h"
#include "RIL_control_panel.h"
#include "RIL_memory.h"
#include "RIL_led_panel.h"
#include "AT24Cxx.h"

typedef struct {
    uint8_t channels[2][2];
    uint32_t settings_numb[2];
} HostileSettings;

extern Button_t button_1, button_2, button_3;
extern ChannelSettings channelSettings;
extern BrightnessSettings brightnessSettings;

bool flag_menu = true, flag_enter_func = true;
HostileSettings hostile;
char *brightness_matrix[16] = {"d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7", "d8", "d9", "d10", "d11","d12", "d13", "d14", "d15"};
uint8_t time_of_day_index = 0;

Menu_Item_t menu_null = {(void*)0, (void*)0, (void*)0, (void*)0};
Menu_Item_t *current_menu_item;

void hostile_settings(const uint8_t *channel, uint8_t hostile[][2]);
void f_menu_item_0(void);
void f_menu_item_1(void);
void f_menu_item_2(void);
void f_menu_item_3(void);
void f_menu_item_4 (void);
void f_menu_item_5 (void);
void f_menu_item_6 (void);
void f_menu_item_7 (void);
void f_menu_item_8 (void);
void f_menu_item_9 (void);
void f_menu_item_10 (void);
void f_menu_item_11 (void);
void f_menu_item_12 (void);
void f_menu_item_13 (void);
void f_menu_item_14 (void);
void f_menu_item_15 (void);
void f_menu_item_16 (void);
void f_menu_item_17 (void);
void f_menu_item_18 (void);
void f_menu_item_19 (void);

/*           Name,         Next,     Parent,     Child,     SelectFunc,  EnterFunc          */
MAKE_MENU(menu_item_0, MENU_NULL, MENU_NULL, MENU_NULL, f_menu_item_0, FUNC_NULL);              //begin

MAKE_MENU(menu_item_1, menu_item_9, MENU_NULL, menu_item_2, f_menu_item_1, FUNC_NULL);          //channels
MAKE_MENU(menu_item_2, MENU_NULL, menu_item_1, menu_item_3, f_menu_item_2, FUNC_NULL);          //0101
MAKE_MENU(menu_item_3, MENU_NULL, menu_item_1, menu_item_4, f_menu_item_3, FUNC_NULL);          //blink 0101
MAKE_MENU(menu_item_4, MENU_NULL, menu_item_1, MENU_NULL, f_menu_item_4, FUNC_NULL);            //delete 0001 0100
MAKE_MENU(menu_item_5, MENU_NULL, menu_item_1, menu_item_6, f_menu_item_5, FUNC_NULL);          //0 - 629
MAKE_MENU(menu_item_6, MENU_NULL, MENU_NULL, MENU_NULL, FUNC_NULL, f_menu_item_6);              //1-38 39-629
MAKE_MENU(menu_item_7, menu_item_8, MENU_NULL, menu_item_5, f_menu_item_7, FUNC_NULL);          //hor
MAKE_MENU(menu_item_8, menu_item_7, MENU_NULL, menu_item_5, f_menu_item_8, FUNC_NULL);          //ver

MAKE_MENU(menu_item_9, menu_item_18, MENU_NULL, menu_item_10, f_menu_item_9, FUNC_NULL);        //L
MAKE_MENU(menu_item_10, menu_item_11, menu_item_9, menu_item_14, f_menu_item_10, FUNC_NULL);    //Ld
MAKE_MENU(menu_item_11, menu_item_12, menu_item_9, menu_item_15, f_menu_item_11, FUNC_NULL);    //Lg
MAKE_MENU(menu_item_12, menu_item_13, menu_item_9, menu_item_16, f_menu_item_12, FUNC_NULL);    //Lt
MAKE_MENU(menu_item_13, menu_item_10, menu_item_9, menu_item_17, f_menu_item_13, FUNC_NULL);    //Ln
MAKE_MENU(menu_item_14, MENU_NULL, MENU_NULL, MENU_NULL, FUNC_NULL, f_menu_item_14);
MAKE_MENU(menu_item_15, MENU_NULL, MENU_NULL, MENU_NULL, FUNC_NULL, f_menu_item_15);
MAKE_MENU(menu_item_16, MENU_NULL, MENU_NULL, MENU_NULL, FUNC_NULL, f_menu_item_16);
MAKE_MENU(menu_item_17, MENU_NULL, MENU_NULL, MENU_NULL, FUNC_NULL, f_menu_item_17);

MAKE_MENU(menu_item_18, menu_item_1, MENU_NULL, menu_item_19, f_menu_item_18, FUNC_NULL);
MAKE_MENU(menu_item_19, menu_item_18, menu_item_18, menu_item_18, f_menu_item_19, FUNC_NULL);


static void menu_change(Menu_Item_t *new_menu) {
    if (new_menu != &MENU_NULL)
        current_menu_item = new_menu;

    if (current_menu_item->SelectFunc != FUNC_NULL)
        current_menu_item->SelectFunc();
}

static void menu_handler_buttons(void) {
    if (button_short_press(&button_1)) {
        menu_change(current_menu_item->Child);
    }
    if (button_short_press(&button_3)) {
        menu_change(current_menu_item->Parent);
    }
    if (button_short_press(&button_2)) {
        menu_change(current_menu_item->Next);
    }
}

void menu_start(void) {
    if (flag_menu) {
        led_yellow_on();
        digit_indicator_start();
        menu_change(&menu_item_1);
        flag_menu = false;
    }

    if (current_menu_item->EnterFunc == FUNC_NULL) {
        menu_handler_buttons();
    } else {
        current_menu_item->EnterFunc();
    }
}

void menu_stop(void) {
    menu_change(&menu_item_0);
    flag_menu = true;

    led_panel_test_off();

    if (current_menu_item->SelectFunc != FUNC_NULL) {
        current_menu_item->SelectFunc();
    }
}


void f_menu_item_0(void) {
    digit_indicator_not_blink();
    digit_indicator_not_blink_all();
    digit_indicator_stop();
}

void f_menu_item_1(void) {
    digit_indicator_not_blink();
    digit_indicator_not_blink_all();
    digit_indicator_print("port", NULL);
}

void f_menu_item_2(void) {
    digit_indicator_print_ports(channelSettings.channels);
}

void f_menu_item_3(void) {
    bool flag_setting_found = false;
    uint32_t plug = 0;

    memset(hostile.channels[0], 0x00, sizeof(hostile.channels[0]));
    memset(hostile.channels[1], 0x00, sizeof(hostile.channels[1]));
    memset(hostile.settings_numb, 0x00, sizeof(hostile.settings_numb));

    if (!mem_search_channel_settings(&plug, channelSettings.channels, SIZE_CHANNEL)) {
        hostile_settings(channelSettings.channels, hostile.channels);

        for (int i = 0; i < 2; ++i) {
            if (hostile.channels[i][0] || hostile.channels[i][1]) {
                if (mem_search_channel_settings(&hostile.settings_numb[i], hostile.channels[i], sizeof(hostile.channels[i])))
                    flag_setting_found = true;
                else memset(hostile.channels[i], 0x00, sizeof(hostile.channels[i]));
            }
        }
    }

    if (flag_setting_found)
        digit_indicator_blink_all();
    else
        menu_change(&menu_item_5);
}

void f_menu_item_4(void) {
    for (int i = 0; i < sizeof(hostile.channels[i]); ++i) {
        if (hostile.channels[i][0] || hostile.channels[i][1])
            channelSettings.setting_num = mem_delete_settings(hostile.settings_numb[i]);
    }

    digit_indicator_not_blink_all();
    menu_item_3.SelectFunc();
}

void f_menu_item_5(void) {
       digit_indicator_print(
               channelSettings.combination.ui16 < 10? "00%d" :
               channelSettings.combination.ui16 <100? "0%d" : "%d", &channelSettings.combination.ui16);
}

void f_menu_item_6(void) {
    static uint16_t begin_value = 0, end_value = 0;

    if (flag_enter_func) {
        if (channelSettings.channels[0] == channelSettings.channels[1]) {
            begin_value = 39;
            end_value = 608;
        } else {
            begin_value = 1;
            end_value = 38;
        }

        if (channelSettings.combination.ui16 == 0)
            channelSettings.combination.ui16 = begin_value;

        digit_indicator_print(
                channelSettings.combination.ui16 < 10? "00%d" :
                channelSettings.combination.ui16 <100? "0%d" : "%d", &channelSettings.combination.ui16);

        digit_indicator_blink_next();
        flag_enter_func = false;
    }

    if (button_short_press(&button_3)) {
        digit_indicator_blink_next();
    }

    if (button_short_press(&button_2)) {
        digit_indicator_increment(begin_value, end_value);
    }

    if (button_short_press(&button_1)) {
        channelSettings.combination.ui16 = str_to_uint(digit_indicator_get_string());
        digit_indicator_not_blink();
        if (channelSettings.combination.ui16 > 38 || channelSettings.combination.ui16 < 5) {
            menu_change(&menu_item_5);
            channelSettings.orientation = HORIZONTALLY;
        } else {
            menu_change(&menu_item_7);
        }
        flag_enter_func = true;
    }
}

void f_menu_item_7(void) {
    digit_indicator_print("hor", NULL);
    channelSettings.orientation = HORIZONTALLY;
}

void f_menu_item_8(void) {
    digit_indicator_print("uer", NULL);
    channelSettings.orientation = VERTICAL;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void f_menu_item_9(void) {
    digit_indicator_print("l", NULL);
}

void f_menu_item_10(void) {
    digit_indicator_print("ld", NULL);
    time_of_day_index = brightnessSettings.daytime_brightness;
}

void f_menu_item_11(void) {
    digit_indicator_print("lg", NULL);
    time_of_day_index = brightnessSettings.cloudy_brightness;
}

void f_menu_item_12(void) {
    digit_indicator_print("lt", NULL);
    time_of_day_index = brightnessSettings.mist_brightness;
}

void f_menu_item_13(void) {
    digit_indicator_print("ln", NULL);
    time_of_day_index = brightnessSettings.night_brightness;
}

//////////////////////////////////////////////////////////////////

void f_menu_item_14(void) {
    if (flag_enter_func) {
        digit_indicator_print(brightness_matrix[time_of_day_index], NULL);
        flag_enter_func = false;
    }

    if (button_short_press(&button_2)) {
        --time_of_day_index;
        if (time_of_day_index < 12)
            time_of_day_index = 15;
        flag_enter_func = true;
    }

    if (button_short_press(&button_3)) {
        flag_enter_func = true;
        menu_change(&menu_item_10);
    }

    if (button_short_press(&button_1)) {
        brightnessSettings.daytime_brightness = time_of_day_index;
        flag_enter_func = true;
        menu_change(&menu_item_10);
    }
}

void f_menu_item_15(void) {
    if (flag_enter_func) {
        digit_indicator_print(brightness_matrix[time_of_day_index], NULL);
        flag_enter_func = false;
    }

    if (button_short_press(&button_2)) {
        --time_of_day_index;
        if (time_of_day_index < 8)
            time_of_day_index = 11;
        flag_enter_func = true;
    }

    if (button_short_press(&button_3)) {
        flag_enter_func = true;
        menu_change(&menu_item_11);
    }

    if (button_short_press(&button_1)) {
        brightnessSettings.cloudy_brightness = time_of_day_index;
        flag_enter_func = true;
        menu_change(&menu_item_11);
    }
}

void f_menu_item_16(void) {
    if (flag_enter_func) {
        digit_indicator_print(brightness_matrix[time_of_day_index], NULL);
        flag_enter_func = false;
    }

    if (button_short_press(&button_2)) {
        --time_of_day_index;
        if (time_of_day_index < 4)
            time_of_day_index = 7;
        flag_enter_func = true;
    }

    if (button_short_press(&button_3)) {
        flag_enter_func = true;
        menu_change(&menu_item_12);
    }

    if (button_short_press(&button_1)) {
        brightnessSettings.mist_brightness = time_of_day_index;
        flag_enter_func = true;
        menu_change(&menu_item_12);
    }
}

void f_menu_item_17(void) {

    if (flag_enter_func) {
        digit_indicator_print(brightness_matrix[time_of_day_index], NULL);
        flag_enter_func = false;
    }

    if (button_short_press(&button_2)) {
        --time_of_day_index;
        if (time_of_day_index == UINT8_MAX)
            time_of_day_index = 3;
        flag_enter_func = true;
    }

    if (button_short_press(&button_3)) {
        flag_enter_func = true;
        menu_change(&menu_item_13);
    }

    if (button_short_press(&button_1)) {
        brightnessSettings.night_brightness = time_of_day_index;
        flag_enter_func = true;
        menu_change(&menu_item_13);
    }
}

void f_menu_item_18(void) {
    digit_indicator_print("test", NULL);
    led_panel_test_off();
    led_panel_clean();
}

void f_menu_item_19 (void) {
    led_panel_test_on();
}

void hostile_settings(const uint8_t *channel, uint8_t hostile_channel[][2]) {
    if (channel[0] == channel[1]) {
        hostile_channel[0][0] = 0;
        hostile_channel[0][1] = channel[0];
        hostile_channel[1][0] = channel[0];
        hostile_channel[1][1] = 0;
    } else if (channel[0]) {
        hostile_channel[0][0] = channel[0];
        hostile_channel[0][1] = channel[0];
    } else {
        hostile_channel[0][0] = channel[1];
        hostile_channel[0][1] = channel[1];
    }
}
