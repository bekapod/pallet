#ifndef PALLET_TEXT_H

#include "config.h"
#define PALLET_TEXT_H

#include <stdint.h>

#ifndef TEXT_TILE_BASE
#define TEXT_TILE_BASE 1
#endif

#ifndef TEXT_EMPTY_TILE
#define TEXT_EMPTY_TILE 0
#endif

#ifndef TEXT_QUEUE_CAPACITY
#endif

/* Sprite tiles reserved for text glyphs and the menu's > cursor. Override
 * before inclusion when a game reserves either range for its own sprite data. */
#ifndef TEXT_SPRITE_TILE_BASE
#define TEXT_SPRITE_TILE_BASE 0xC0U
#endif

#ifndef TEXT_CURSOR_TILE
#define TEXT_CURSOR_TILE 0xFEU
#endif

#define TEXT_FONT_TILES 40

void text_init(const uint8_t *font_tiles);
void text_vblank(void);

/* Queue text for the background tile map; bg_flush() applies it. */
void text_print(uint8_t x, uint8_t y, const char *text);

/* Queue text for the window tile map; text_vblank() applies it. */
void text_window_print(uint8_t x, uint8_t y, const char *text);
void text_digits(uint8_t x, uint8_t y, uint16_t value, uint8_t width);

/* Return the sprite tile for a supported glyph, or TEXT_EMPTY_TILE. */
uint8_t text_sprite_tile(char character);

#endif
