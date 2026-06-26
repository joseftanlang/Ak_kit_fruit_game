#ifndef __FRUITS_H__
#define __FRUITS_H__

#include <stdint.h>

#include "../screens/scr_fruit_game.h"

typedef struct {
    int16_t x;
    int16_t y;
    uint8_t type;
} fruit_object_t;

void fruits_init(void);
void fruits_update(void);
fruit_object_t* fruits_get(void);

/* Returns the bitmap for a fruit type (0-5) */
const uint8_t* fruits_bitmap_get(uint8_t type);

#endif