#ifndef PALLET_CAMERA_H
#define PALLET_CAMERA_H

#include <stdint.h>

/* The camera is the only place that writes SCX_REG and SCY_REG.
 * It takes the scroll position and any shake offset and combines them
 * into one register write per frame. */

/* Set the base vertical scroll position in pixels. The horizontal base
 * comes from the scroll module's world position. */
void camera_set_y(uint8_t y);

/* Start a screen shake of the given duration in frames and pixel
 * amplitude. A zero duration immediately clears any active shake. */
void camera_shake(uint8_t frames, uint8_t amplitude);

/* Advance shake timing. Call once per frame, e.g. from the VBlank tick. */
void camera_tick(void);

/* Compose the scroll base, camera base Y, and shake offsets, then write
 * SCX_REG and SCY_REG. Call once per frame after camera_tick(). */
void camera_apply(void);

#endif