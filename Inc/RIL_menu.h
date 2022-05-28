
#ifndef ROUT_INDICATOR_LIGHT_RIL_MENU_H
#define ROUT_INDICATOR_LIGHT_RIL_MENU_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"

typedef void (*FuncPtr)(void);

/*
 * Описание структуры пункта меню
 */
typedef struct Menu_Item {
    struct Menu_Item    *Next;
    struct Menu_Item    *Parent;
    struct Menu_Item    *Child;
    FuncPtr             SelectFunc;
    FuncPtr             EnterFunc;
} Menu_Item_t;

extern Menu_Item_t  menu_null;
extern Menu_Item_t  *current_menu_item;

#define MENU_NULL  menu_null
#define FUNC_NULL  NULL

/*
 * Макрос для быстрого и удобного определения меню
 */
#define MAKE_MENU(Name, Next, Parent, Child, SelectFunc, EnterFunc) \
    extern Menu_Item_t Next;   \
	extern Menu_Item_t Parent; \
	extern Menu_Item_t Child;  \
	Menu_Item_t Name = {&(Next), &(Parent), &(Child), (FuncPtr)(SelectFunc), (FuncPtr)(EnterFunc)}

void menu_start(void);
void menu_stop(void);

#ifdef __cplusplus
}
#endif

#endif //ROUT_INDICATOR_LIGHT_RIL_MENU_H
