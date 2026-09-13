#ifndef PALLET_HIT_H
#define PALLET_HIT_H

#include <stdint.h>

/* Half-open screen-space rectangle: [x, x + w) and [y, y + h). */
typedef struct {
    uint8_t x;
    uint8_t y;
    uint8_t w;
    uint8_t h;
} hitbox_t;

/* True when the two rectangles share at least one pixel. */
uint8_t hit_overlaps(hitbox_t a, hitbox_t b);

/* True when the point lies inside the rectangle. */
uint8_t hit_contains(hitbox_t box, uint8_t x, uint8_t y);

#endif