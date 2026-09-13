#ifndef PALLET_TEXT_H
#define PALLET_TEXT_H

#include <stdint.h>

#ifndef TEXT_TILE_BASE
#define TEXT_TILE_BASE 1
#endif

#ifndef TEXT_EMPTY_TILE
#define TEXT_EMPTY_TILE 0
#endif

#ifndef TEXT_QUEUE_CAPACITY
#define TEXT_QUEUE_CAPACITY 64
#endif

#define TEXT_FONT_TILES 40

void text_init(const uint8_t *font_tiles);
void text_vblank(void);

/* Queue text for the background tile map; text_vblank() applies it. */
void text_print(uint8_t x, uint8_t y, const char *text);

/* Queue text for the window tile map; text_vblank() applies it. */
void text_window_print(uint8_t x, uint8_t y, const char *text);
void text_digits(uint8_t x, uint8_t y, uint16_t value, uint8_t width);

#endif
