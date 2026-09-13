#ifndef PALLET_FX_H
#define PALLET_FX_H

#include <stdint.h>

/* Signed 8.8 fixed-point values. FX(n) accepts whole numbers from -128 to 127. */
typedef int16_t fx_t;

/* Convert a whole number to 8.8 fixed point without signed-shifting. */
#define FX(n) ((fx_t)((n) * 256))
#define FX_INT(v) ((int8_t)((v) >> 8))
#define FX_FRAC(v) ((uint8_t)(v))

/* Multiply by an unsigned 0.8 scalar: 128 is one half and 255 is below one. */
#define FX_MUL_U8(v, k) ((fx_t)(((int32_t)(v) * (k)) >> 8))

#define FX_SIGN(v) (((v) < 0) ? -1 : ((v) > 0) ? 1 : 0)
#define FX_CLAMP(v, low, high) \
    (((v) < (low)) ? (low) : (((v) > (high)) ? (high) : (v)))

/* Value expressions passed to comparison macros must be side-effect-free.
 * FX_CLAMP also requires low <= high. */

#endif
