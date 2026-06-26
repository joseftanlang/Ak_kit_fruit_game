#ifndef __BASKET_H__
#define __BASKET_H__

#include <stdint.h>
#include "../screens/scr_fruit_game.h"

void basket_init(void);

void basket_left(void);
void basket_right(void);

int16_t basket_get_x(void);

#endif