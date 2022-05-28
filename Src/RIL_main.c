#include <string.h>
#include <stdbool.h>
#include "stm32f1xx_hal.h"
#include "RIL_main.h"
#include "RIL_menu.h"
#include "RIL_memory.h"
#include "RIL_control_panel.h"
#include "RIL_led_panel.h"
#include "RIL_channels.h"

extern Button_t button_1;

OperatingMode ril_mode = EXPLOITATION_MODE;
PictureArray pictureArray;
ChannelSettings channelSettings;
BrightnessSettings brightnessSettings;
uint8_t physical_ports[2] = {0};
bool flag_settings = false, flag_error = true;

char *list_combinations[608] = { //629
        "arrow straight", "arrow left", "arrow right", "KGU",

        "A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X",
        "0","1","2","3","4","5","6","7","8","9","10","11","12","13","14","15","16","17","18","19","20", "21","22","23",
        "24","25","26","27","28","29","30","31","32","33","34","35","36","37","38","39","40","41","42","43","44","45",
        "46","47","48","49","50","51","52","53","54","55","56","57","58","59","60","61","62","63","64","65","66","67",
        "68","69","70","71","72","73","74","75","76","77","78","79","80","81","82","83","84","85","86","87","88","89",
        "90","91","92","93","94","95","96","97","98","99",

        "A0","A1","A2","A3","A4","A5","A6","A7","A8","A9",
        "B0","B1","B2","B3", "B4","B5","B6","B7","B8","B9",
        "C0","C1","C2","C3","C4","C5","C6","C7","C8","C9",
        "D0","D1","D2","D3","D4","D5","D6","D7","D8","D9",
        "E0","E1","E2","E3","E4","E5","E6","E7","E8","E9",
        "F0","F1","F2","F3","F4","F5","F6","F7","F8","F9",
        "G0","G1","G2","G3","G4","G5","G6","G7","G8","G9",
        "H0","H1","H2","H3","H4","H5","H6","H7","H8","H9",
        "I0","I1","I2","I3","I4","I5","I6","I7","I8","I9",
        "J0","J1","J2","J3","J4","J5","J6","J7","J8","J9",
        "K0","K1","K2","K3","K4","K5","K6","K7","K8","K9",
        "L0","L1","L2","L3","L4","L5","L6","L7","L8","L9",
        "M0","M1","M2","M3","M4","M5","M6","M7","M8","M9",
        "N0","N1","N2","N3","N4","N5","N6","N7","N8","N9",
        "O0","O1","O2","O3","O4","O5","O6","O7","O8","O9",
        "P0","P1","P2","P3","P4","P5","P6","P7","P8","P9",
        "Q0","Q1","Q2","Q3","Q4","Q5","Q6","Q7","Q8","Q9",
        "R0","R1","R2","R3","R4","R5","R6","R7","R8","R9",
        "S0","S1","S2","S3","S4","S5","S6","S7","S8","S9",
        "T0","T1","T2","T3","T4","T5","T6","T7","T8","T9",
        "U0","U1","U2","U3","U4","U5","U6","U7","U8","U9",
        "V0","V1","V2","V3","V4","V5","V6","V7","V8","V9",
        "W0","W1","W2","W3","W4","W5","W6","W7","W8","W9",
        "X0","X1","X2","X3","X4","X5","X6","X7","X8","X9",

        "0A","1A","2A","3A","4A","5A","6A","7A","8A","9A",
        "0B","1B","2B","3B","4B","5B","6B","7B","8B","9B",
        "0C","1C","2C","3C","4C","5C","6C","7C","8C","9C",
        "0D","1D","2D","3D","4D","5D","6D","7D","8D","9D",
        "0E","1E","2E","3E","4E","5E","6E","7E","8E","9E",
        "0F","1F","2F","3F","4F","5F","6F","7F","8F","9F",
        "0G","1G","2G","3G","4G","5G","6G","7G","8G","9G",
        "0H","1H","2H","3H","4H","5H","6H","7H","8H","9H",
        "0I","1I","2I","3I","4I","5I","6I","7I","8I","9I",
        "0J","1J","2J","3J","4J","5J","6J","7J","8J","9J",
        "0K","1K","2K","3K","4K","5K","6K","7K","8K","9K",
        "0L","1L","2L","3L","4L","5L","6L","7L","8L","9L",
        "0M","1M","2M","3M","4M","5M","6M","7M","8M","9M",
        "0N","1N","2N","3N","4N","5N","6N","7N","8N","9N",
        "0O","1O","2O","3O","4O","5O","6O","7O","8O","9O",
        "0P","1P","2P","3P","4P","5P","6P","7P","8P","9P",
        "0Q","1Q","2Q","3Q","4Q","5Q","6Q","7Q","8Q","9Q",
        "0R","1R","2R","3R","4R","5R","6R","7R","8R","9R",
        "0S","1S","2S","3S","4S","5S","6S","7S","8S","9S",
        "0T","1T","2T","3T","4T","5T","6T","7T","8T","9T",
        "0U","1U","2U","3U","4U","5U","6U","7U","8U","9U",
        "0V","1V","2V","3V","4V","5V","6V","7V","8V","9V",
        "0W","1W","2W","3W","4W","5W","6W","7W","8W","9W",
        "0X","1X","2X","3X","4X","5X","6X","7X","8X","9X",
};


static bool channels_ok (void) {
    channels_read(physical_ports);
    if (((physical_ports[0] <= 11) && (physical_ports[1] <= 11)) && (
            (physical_ports[0] == physical_ports[1]) || ((physical_ports[0] == 0) && physical_ports[1]) || (physical_ports[0] && (physical_ports[1] == 0))))
        return true;
    else
        return false;
}


static bool checking_channel_change(void) {
    return (memcmp(physical_ports, channelSettings.channels, SIZE_CHANNEL) != 0) ? true : false;
}

static bool checking_settings_change(void) {
    if (flag_settings) {
        flag_settings = false;
        return true;
    } else return false;
}

void RIL_init(void) {
    channels_init();
    led_panel_init();
    digit_indicator_init();
    buttons_init();

    mem_read_brightness_settings(&brightnessSettings);
    led_panel_change_brightness(&brightnessSettings);

    channelSettings.orientation = HORIZONTALLY;
    channelSettings.combination.ui16 = 0;
    memset(channelSettings.channels, 0, SIZE_CHANNEL);
}

void RIL_run(void) {
    if (!channels_ok()) {
        ril_mode = ERROR_MODE;
    }

    switch (ril_mode) {
        case EXPLOITATION_MODE: {
            if (checking_channel_change() || checking_settings_change() || !flag_error) {
                memcpy(channelSettings.channels, physical_ports, SIZE_CHANNEL);

                led_green_on();

                if (mem_search_channel_settings(&channelSettings.setting_num, channelSettings.channels, SIZE_CHANNEL)) {
                    mem_read_combOrien_setting(&channelSettings);

                    if (channelSettings.combination.ui16 && channelSettings.combination.ui16 <= 608) {
                        if (channelSettings.combination.ui16 >= 5 && channelSettings.combination.ui16 <= 38 && channelSettings.orientation == HORIZONTALLY) {
                            mem_read_array_1char_hor(&pictureArray, list_combinations[channelSettings.combination.ui16 - 1]);
                        } else {
                            mem_read_array(&pictureArray, list_combinations[channelSettings.combination.ui16 - 1],
                                           strlen(list_combinations[channelSettings.combination.ui16 - 1]));
                        }
                        led_panel_print(&pictureArray);
                    } else {
                        led_panel_clean();
                        channelSettings.combination.ui16 = 0;
                    }
                } else {
                    led_panel_clean();
                    channelSettings.combination.ui16 = 0;
                }
                flag_error = true;
            }

            if (led_panel_change_brightness_with_delay(&brightnessSettings) && channelSettings.combination.ui16) {
                led_panel_print(&pictureArray);
            }

            if (button_long_press(&button_1)) {
                led_panel_clean();
                ril_mode = SETTING_MODE;
            }
            break;
        }
        case SETTING_MODE: {
            menu_start();
            if (button_long_press(&button_1)) {
                menu_stop();
                mem_write_brightness_settings(&brightnessSettings);
                led_panel_change_brightness(&brightnessSettings);
                ril_mode = EXPLOITATION_MODE;
                flag_settings = true;
                if (channelSettings.combination.ui16 != 0) {
                    mem_write_channel_settings(&channelSettings);
                }
            }
            break;
        }
        case ERROR_MODE: {
            if (flag_error) {
                led_red_on();
                led_panel_clean();
                digit_indicator_start();
                digit_indicator_not_blink_all();
                digit_indicator_not_blink();
                digit_indicator_print("f1", NULL);
                flag_error = false;
            }
            if (channels_ok()) {
                ril_mode = EXPLOITATION_MODE;
                digit_indicator_stop();
            }
            break;
        }
        default:;
    }

}
