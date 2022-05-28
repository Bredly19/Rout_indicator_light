#ifndef ROUT_INDICATOR_LIGHT_RIL_MAIN_H
#define ROUT_INDICATOR_LIGHT_RIL_MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

extern TIM_HandleTypeDef htim1;

typedef enum {
    EXPLOITATION_MODE = 0,
    SETTING_MODE,
    ERROR_MODE
} OperatingMode ;

void RIL_init(void);
void RIL_run(void);

#ifdef __cplusplus
}
#endif

#endif //ROUT_INDICATOR_LIGHT_RIL_MAIN_H
