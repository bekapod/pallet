#ifndef PALLET_RNG_H
#define PALLET_RNG_H

#include <stdint.h>

typedef struct {
    uint16_t s;
} rng_t;

extern rng_t rng_global;

void rng_init(rng_t *rng, uint16_t seed);
uint16_t rng_next(rng_t *rng);

/* Return zero for a zero bound. Otherwise return a value below n. */
uint16_t rng_range(rng_t *rng, uint16_t n);

/* Return true when a 0..99 roll is below percent. 100 or more always wins. */
uint8_t rng_chance(rng_t *rng, uint8_t percent);

#endif
