#ifndef PALLET_SHAKE_H
#define PALLET_SHAKE_H

#include <stdint.h>

extern uint8_t shake_active;
/* How far the screen is currently shifted by a shake, in pixels.
 * Both are 0 when no shake is running. */
extern int8_t shake_offset_x;
extern int8_t shake_offset_y;

void shake(uint8_t frames, uint8_t amplitude);
void shake_tick(void);

#endif