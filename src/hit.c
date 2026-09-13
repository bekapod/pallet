#include "hit.h"

uint8_t hit_aabb(uint8_t ax, uint8_t ay, uint8_t aw, uint8_t ah,
                 uint8_t bx, uint8_t by, uint8_t bw, uint8_t bh) {
    return (uint8_t)(ax < bx + bw && bx < ax + aw && ay < by + bh &&
                     by < ay + ah);
}

uint8_t hit_point_in(uint8_t x, uint8_t y,
                     uint8_t bx, uint8_t by, uint8_t bw, uint8_t bh) {
    return (uint8_t)(x >= bx && x < bx + bw && y >= by && y < by + bh);
}
