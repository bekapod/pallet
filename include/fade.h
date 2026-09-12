#ifndef PALLET_FADE_H
#define PALLET_FADE_H

#include <stdint.h>

extern uint8_t fade_active;

void fade_out(uint8_t frames_per_step);
void fade_in(uint8_t frames_per_step);
void fade_tick(void);

#endif
