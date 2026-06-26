#include "scr_fruit_game.h"

#include <stdlib.h>

#include "screens.h"
#include "app_eeprom.h"

#include "game/fruits.h"
#include "game/basket.h"

#define FRUIT_GAME_TICK_SIG              (AK_USER_DEFINE_SIG + 50)
#define FRUIT_GAME_TICK_INTERVAL_MS      (110)
#define FRUIT_GAME_COUNTDOWN_SIG         (AK_USER_DEFINE_SIG + 51)
#define FRUIT_GAME_COUNTDOWN_INTERVAL_MS (1000)

/*----------------------------------------------------------
 * Global game state
 *---------------------------------------------------------*/

uint8_t fruit_game_state;
fruit_game_setting_t fruit_settingsetup;

/* NOTE:
 * These are NOT static because fruits.cpp uses them.
 */
int16_t fruit_game_score = 0;
uint8_t good_collected_mask = 0;
uint8_t bad_collected_mask = 0;

static uint16_t fruit_game_countdown_seconds = 30;

/*----------------------------------------------------------
 * Utility
 *---------------------------------------------------------*/

uint8_t fruit_game_is_overlap_x(
    int16_t x1,
    int16_t w1,
    int16_t x2,
    int16_t w2
) {
    if (x1 + w1 < x2) return 0;
    if (x2 + w2 < x1) return 0;
    return 1;
}

/*----------------------------------------------------------
 * Reset
 *---------------------------------------------------------*/

static void fruit_game_reset(void) {

    fruit_game_score = 0;
    good_collected_mask = 0;
    bad_collected_mask = 0;

    ar_game_setting_read(&settingdata);

    fruit_settingsetup.speed = settingdata.meteoroid_speed;

    basket_init();
    fruits_init();

    fruit_game_countdown_seconds = 30;

    fruit_game_state = FRUIT_GAME_PLAY;
}

/*----------------------------------------------------------
 * Save score
 *---------------------------------------------------------*/

static void fruit_game_save_score(void) {

    ar_game_score_t score;

    uint32_t score_save =
        (fruit_game_score > 0) ?
        (uint32_t)fruit_game_score :
        0;

    ar_game_score_read(&score);

    score.score_now = score_save;

    if (score.score_now > score.score_1st) {

        score.score_3rd = score.score_2nd;
        score.score_2nd = score.score_1st;
        score.score_1st = score.score_now;

    }
    else if (score.score_now > score.score_2nd) {

        score.score_3rd = score.score_2nd;
        score.score_2nd = score.score_now;

    }
    else if (score.score_now > score.score_3rd) {

        score.score_3rd = score.score_now;
    }

    ar_game_score_write(&score);
}

/*----------------------------------------------------------
 * Draw screen
 *---------------------------------------------------------*/

static void view_scr_fruit_game(void) {

    view_render.clear();

    view_render.setTextSize(1);
    view_render.setTextColor(WHITE);

    /* Score */
    view_render.setCursor(2, 2);
    view_render.print("Fruit:");
    view_render.print(fruit_game_score);

    /* Timer */
    view_render.setCursor(100, 2);
    view_render.print("T:");

    if (fruit_game_countdown_seconds < 10) {
        view_render.print("0");
    }

    view_render.print(fruit_game_countdown_seconds);

    /* Top line */
    view_render.drawLine(
        0,
        FRUIT_GAME_TOP_UI_H - 1,
        LCD_WIDTH,
        FRUIT_GAME_TOP_UI_H - 1,
        WHITE
    );

    /* Falling fruit */
    fruit_object_t *fruit = fruits_get();

    view_render.drawBitmap(
        fruit->x,
        fruit->y,
        fruits_bitmap_get(fruit->type),
        FRUIT_GAME_FRUIT_W,
        FRUIT_GAME_FRUIT_H,
        WHITE
    );

    /* Basket */
    view_render.drawBitmap(
        basket_get_x(),
        FRUIT_GAME_BASKET_Y,
        bitmap_fruit_basket,
        FRUIT_GAME_BASKET_W,
        FRUIT_GAME_BASKET_H,
        WHITE
    );

    /* Border */
    view_render.drawRect(
        0,
        0,
        LCD_WIDTH,
        LCD_HEIGHT,
        WHITE
    );

    /* End screen */
    if (fruit_game_state == FRUIT_GAME_OVER ||
        fruit_game_state == FRUIT_GAME_WIN) {

        view_render.fillRect(
            12,
            28,
            104,
            32,
            BLACK
        );

        view_render.drawRect(
            12,
            28,
            104,
            32,
            WHITE
        );

        view_render.setCursor(22, 34);

        if (fruit_game_state == FRUIT_GAME_WIN) {

            view_render.print("You Win!");

            view_render.setCursor(20, 44);
            view_render.print("Score: ");
            view_render.print(fruit_game_score);

        } else {

            view_render.print("Game Over!");

            view_render.setCursor(20, 44);

            if (fruit_game_countdown_seconds == 0) {
                view_render.print("Time Out!");
            } else {
                view_render.print("3 Bad Fruits!");
            }
        }
    }
}

/*----------------------------------------------------------
 * Screen objects
 *---------------------------------------------------------*/

view_dynamic_t dyn_view_item_fruit_game = {
    {
        .item_type = ITEM_TYPE_DYNAMIC,
    },
    view_scr_fruit_game
};

view_screen_t scr_fruit_game = {
    &dyn_view_item_fruit_game,
    ITEM_NULL,
    ITEM_NULL,

    .focus_item = 0,
};

/*----------------------------------------------------------
 * Main handler
 *---------------------------------------------------------*/

void scr_fruit_game_handle(ak_msg_t *msg) {

    switch (msg->sig) {

    case SCREEN_ENTRY: {

        view_render.initialize();
        view_render_display_on();

        fruit_game_reset();

        timer_set(
            AC_TASK_DISPLAY_ID,
            FRUIT_GAME_TICK_SIG,
            FRUIT_GAME_TICK_INTERVAL_MS,
            TIMER_PERIODIC
        );

        timer_set(
            AC_TASK_DISPLAY_ID,
            FRUIT_GAME_COUNTDOWN_SIG,
            FRUIT_GAME_COUNTDOWN_INTERVAL_MS,
            TIMER_PERIODIC
        );

    } break;

    case FRUIT_GAME_TICK_SIG: {

        fruits_update();

    } break;

    case FRUIT_GAME_COUNTDOWN_SIG: {

        if (fruit_game_state == FRUIT_GAME_PLAY &&
            fruit_game_countdown_seconds > 0) {

            fruit_game_countdown_seconds--;

            if (fruit_game_countdown_seconds == 0) {

                fruit_game_state = FRUIT_GAME_OVER;

                timer_remove_attr(
                    AC_TASK_DISPLAY_ID,
                    FRUIT_GAME_TICK_SIG
                );

                BUZZER_PlaySound(BUZZER_SOUND_LOWSCORE);
            }
        }

    } break;

    case AC_DISPLAY_BUTTON_UP_RELEASED: {

        if (fruit_game_state == FRUIT_GAME_PLAY) {
            basket_left();
            BUZZER_PlaySound(BUZZER_SOUND_CLICK);
        }

    } break;

    case AC_DISPLAY_BUTTON_DOWN_RELEASED: {

        if (fruit_game_state == FRUIT_GAME_PLAY) {
            basket_right();
            BUZZER_PlaySound(BUZZER_SOUND_CLICK);
        }

    } break;

    case AC_DISPLAY_BUTTON_MODE_RELEASED: {

        if (fruit_game_state == FRUIT_GAME_OVER ||
            fruit_game_state == FRUIT_GAME_WIN) {

            fruit_game_save_score();

            fruit_game_state = FRUIT_GAME_OFF;

            timer_remove_attr(
                AC_TASK_DISPLAY_ID,
                FRUIT_GAME_TICK_SIG
            );

            timer_remove_attr(
                AC_TASK_DISPLAY_ID,
                FRUIT_GAME_COUNTDOWN_SIG
            );

            SCREEN_TRAN(scr_charts_game_handle, &scr_charts_game);
        }

    } break;

    case AC_DISPLAY_BUTTON_UP_LONG_PRESSED: {

        fruit_game_state = FRUIT_GAME_OFF;

        timer_remove_attr(AC_TASK_DISPLAY_ID, FRUIT_GAME_TICK_SIG);
        timer_remove_attr(AC_TASK_DISPLAY_ID, FRUIT_GAME_COUNTDOWN_SIG);

        SCREEN_TRAN(scr_idle_handle, &scr_idle);

    } break;

    case AC_DISPLAY_BUTTON_DOWN_LONG_PRESSED: {

        fruit_game_state = FRUIT_GAME_OFF;

        timer_remove_attr(AC_TASK_DISPLAY_ID, FRUIT_GAME_TICK_SIG);
        timer_remove_attr(AC_TASK_DISPLAY_ID, FRUIT_GAME_COUNTDOWN_SIG);

        SCREEN_TRAN(scr_menu_game_handle, &scr_menu_game);

    } break;

    default:
        break;
    }
}