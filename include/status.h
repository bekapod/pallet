#ifndef PALLET_STATUS_H
#define PALLET_STATUS_H

#include <stdint.h>

/* Shared status codes for bounded Pallet operations. */
typedef uint8_t pallet_status_t;

#define PALLET_OK   0
#define PALLET_BUSY 1
#define PALLET_FULL 2
#define PALLET_EMPTY 3
#define PALLET_BAD_ARGUMENT 4

#endif