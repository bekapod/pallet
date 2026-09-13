#include "hit.h"

uint8_t hit_overlaps(hitbox_t a, hitbox_t b) {
    return (uint8_t)(a.x < b.x + b.w && b.x < a.x + a.w && a.y < b.y + b.h &&
                     b.y < a.y + a.h);
}

uint8_t hit_contains(hitbox_t box, uint8_t x, uint8_t y) {
    return (uint8_t)(x >= box.x && x < box.x + box.w && y >= box.y &&
                     y < box.y + box.h);
}