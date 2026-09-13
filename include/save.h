#ifndef PALLET_SAVE_H
#define PALLET_SAVE_H

#include <stdint.h>

#define SAVE_MAGIC 0xA5U
/* SRAM layout: magic byte, schema version byte, checksum byte, payload. */
#define SAVE_HEADER_BYTES 3U

typedef uint8_t save_result_t;

#define SAVE_OK           0
#define SAVE_MISSING      1 /* No save present, or wiped. */
#define SAVE_BAD_VERSION  2 /* Save exists but was written by another version. */
#define SAVE_CORRUPT      3 /* Checksum mismatch. */
#define SAVE_BAD_ARGUMENT 4 /* Zero length. */

/* Load `len` payload bytes into dst. The schema version is passed
 * explicitly and never stored inside the caller's data. On any failure
 * dst is left untouched. */
save_result_t save_load(uint8_t version, void *dst, uint8_t len);

/* Write `len` payload bytes from src under the given schema version. */
save_result_t save_write(uint8_t version, const void *src, uint8_t len);

void save_wipe(void);

#endif