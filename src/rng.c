#include "rng.h"

rng_t rng_global;

void rng_init(rng_t *rng, uint16_t seed) {
    rng->s = seed ? seed : 1;
}

uint16_t rng_next(rng_t *rng) {
    uint16_t state = rng->s;

    state ^= (uint16_t)(state << 7);
    state ^= (uint16_t)(state >> 9);
    state ^= (uint16_t)(state << 8);
    rng->s = state;
    return state;
}

uint16_t rng_range(rng_t *rng, uint16_t n) {
    if (!n)
        return 0;
    return rng_next(rng) % n;
}

uint8_t rng_chance(rng_t *rng, uint8_t percent) {
    if (percent >= 100U)
        return 1;
    return (uint8_t)(rng_next(rng) % 100U < percent);
}
