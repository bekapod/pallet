#ifndef PALLET_BG_H
#define PALLET_BG_H

#include <stdint.h>

/* Maximum number of queued commands. Define before including this header to override. */
#ifndef BG_QUEUE_CAPACITY
#define BG_QUEUE_CAPACITY 512U
#endif

/* Maximum number of background-map entries written by one bg_flush(). */
#ifndef BG_FLUSH_TILE_BUDGET
#define BG_FLUSH_TILE_BUDGET 24U
#endif

/* Coordinates address the 32x32 hardware map and wrap at its edges. */
void bg_put(uint8_t x, uint8_t y, uint8_t tile);

/* Queue a non-empty span of at most 32 tiles. The source must remain unchanged
 * until bg_pending() reaches zero. Spans wrap at the corresponding map edge. */
void bg_put_row(uint8_t x, uint8_t y, const uint8_t *tiles, uint8_t len);
void bg_put_col(uint8_t x, uint8_t y, const uint8_t *tiles, uint8_t len);

/* Queue w*h individual entries. The bounded queue drops entries when full;
 * callers requiring delivery should observe bg_pending() before a redraw. */
void bg_fill(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t tile);

/* Apply queued writes during the post-vsync VBlank window only. */
void bg_flush(void);

/* Number of tile-map entries still waiting, not number of command slots. */
uint16_t bg_pending(void);

#endif
