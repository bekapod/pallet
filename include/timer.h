#ifndef PALLET_TIMER_H
#define PALLET_TIMER_H

#include <stdint.h>

/* A caller-owned countdown that fires once per configured period. */
typedef struct {
    uint8_t period;
    uint8_t count;
} timer_t;

/* Set the period and arm a full interval. A zero period disables the timer. */
void timer_start(timer_t *timer, uint8_t period);

/* Tick once per frame. Expiry reloads the timer and returns 1. */
uint8_t timer_tick(timer_t *timer);

/* Re-arm a full interval without changing the configured period. */
void timer_reset(timer_t *timer);

#endif
