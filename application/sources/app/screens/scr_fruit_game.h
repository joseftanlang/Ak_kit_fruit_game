#ifndef __SCR_FRUIT_GAME_H__
#define __SCR_FRUIT_GAME_H__

#include "fsm.h"
#include "port.h"
#include "message.h"
#include "timer.h"

#include "sys_ctrl.h"
#include "sys_dbg.h"

#include "app.h"
#include "app_dbg.h"
#include "task_list.h"
#include "task_display.h"
#include "view_render.h"

#include "buzzer.h"

#include "screens.h"
#include "screens_bitmap.h"

/* Game states */
#define FRUIT_GAME_OFF       (0)
#define FRUIT_GAME_PLAY      (1)
#define FRUIT_GAME_OVER      (2)
#define FRUIT_GAME_WIN       (3)

/* Game layout */
#define FRUIT_GAME_TOP_UI_H      (12)
#define FRUIT_GAME_FRUIT_W       (8)
#define FRUIT_GAME_FRUIT_H       (8)
#define FRUIT_GAME_BASKET_W      (16)
#define FRUIT_GAME_BASKET_H      (8)
#define FRUIT_GAME_BASKET_Y      (54)
#define FRUIT_GAME_BASKET_STEP   (6)

/* Fruit counts */
#define FRUIT_GAME_NUM_FRUITS    (6)
#define FRUIT_GAME_NUM_GOOD      (3)
#define FRUIT_GAME_NUM_BAD       (3)

typedef struct {
    uint8_t speed;
} fruit_game_setting_t;

extern uint8_t fruit_game_state;
extern fruit_game_setting_t fruit_settingsetup;

extern view_dynamic_t dyn_view_item_fruit_game;
extern view_screen_t scr_fruit_game;

void scr_fruit_game_handle(ak_msg_t *msg);

/* Shared helper */
uint8_t fruit_game_is_overlap_x(
    int16_t x1,
    int16_t w1,
    int16_t x2,
    int16_t w2
);

#endif