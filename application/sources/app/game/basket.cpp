#include "basket.h"

static int16_t basket_x = 0;

void basket_init(void) {
    basket_x = (LCD_WIDTH - FRUIT_GAME_BASKET_W) / 2;
}

void basket_left(void) {
    basket_x -= FRUIT_GAME_BASKET_STEP;

    if (basket_x < 0) {
        basket_x = 0;
    }
}

void basket_right(void) {
    basket_x += FRUIT_GAME_BASKET_STEP;

    if (basket_x > (LCD_WIDTH - FRUIT_GAME_BASKET_W)) {
        basket_x = LCD_WIDTH - FRUIT_GAME_BASKET_W;
    }
}

int16_t basket_get_x(void) {
    return basket_x;
}