#include "fruits.h"
#include "basket.h"
#include <stdlib.h>

static fruit_object_t falling_fruit;

extern uint8_t fruit_game_state;
extern fruit_game_setting_t fruit_settingsetup;

extern int16_t fruit_game_score;
extern uint8_t good_collected_mask;
extern uint8_t bad_collected_mask;

extern uint8_t fruit_game_is_overlap_x(
    int16_t x1,
    int16_t w1,
    int16_t x2,
    int16_t w2
);

static const uint8_t* const fruit_bitmap_table[FRUIT_GAME_NUM_FRUITS] = {
    bitmap_fruit_good_1,
    bitmap_fruit_good_2,
    bitmap_fruit_good_3,
    bitmap_fruit_bad_1,
    bitmap_fruit_bad_2,
    bitmap_fruit_bad_3,
};

static void fruits_spawn(void) {
    falling_fruit.type = rand() % FRUIT_GAME_NUM_FRUITS;
    falling_fruit.x = rand() % (LCD_WIDTH - FRUIT_GAME_FRUIT_W);
    falling_fruit.y = FRUIT_GAME_TOP_UI_H;
}

void fruits_init(void) {
    fruits_spawn();
}

fruit_object_t* fruits_get(void) {
    return &falling_fruit;
}

const uint8_t* fruits_bitmap_get(uint8_t type) {
    return fruit_bitmap_table[type];
}

void fruits_update(void) {

    if (fruit_game_state != FRUIT_GAME_PLAY) {
        return;
    }

    falling_fruit.y += fruit_settingsetup.speed;

    if ((falling_fruit.y + FRUIT_GAME_FRUIT_H) >= FRUIT_GAME_BASKET_Y) {

        if (fruit_game_is_overlap_x(
                falling_fruit.x,
                FRUIT_GAME_FRUIT_W,
                basket_get_x(),
                FRUIT_GAME_BASKET_W)) {

            if (falling_fruit.type < FRUIT_GAME_NUM_GOOD) {

                good_collected_mask |= (1U << falling_fruit.type);
                fruit_game_score += 10;
                BUZZER_PlaySound(BUZZER_SOUND_CLICK);

                if (good_collected_mask == 0x07) {
                    fruit_game_state = FRUIT_GAME_WIN;
                }

            } else {

                bad_collected_mask |= (1U << (falling_fruit.type - FRUIT_GAME_NUM_GOOD));
                fruit_game_score -= 5;
                BUZZER_PlaySound(BUZZER_SOUND_LOWSCORE);

                if (bad_collected_mask >= 3) {
                    fruit_game_state = FRUIT_GAME_OVER;
                }
            }

            fruits_spawn();
            return;
        }
    }

    if (falling_fruit.y > LCD_HEIGHT) {
        fruits_spawn();
    }
}