#ifndef PALLET_HIT_H
#define PALLET_HIT_H

#include <stdint.h>

/* Rectangles use half-open screen-space bounds: [x, x + width) and
 * [y, y + height). Callers must provide nonzero widths and heights. */
uint8_t hit_aabb(uint8_t ax, uint8_t ay, uint8_t aw, uint8_t ah,
                 uint8_t bx, uint8_t by, uint8_t bw, uint8_t bh);
uint8_t hit_point_in(uint8_t x, uint8_t y,
                     uint8_t bx, uint8_t by, uint8_t bw, uint8_t bh);

#endif
