#ifndef PALLET_SHAKE_H
#define PALLET_SHAKE_H

#include <stdint.h>

extern uint8_t shake_active;

void shake(uint8_t frames, uint8_t amplitude);
void shake_tick(void);

#endif
