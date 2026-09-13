#include <gb/gb.h>
#include <stdint.h>

#include "bg.h"
#include "text.h"

typedef struct {
    uint8_t x;
    uint8_t y;
    uint8_t tile;
} text_write_t;

static text_write_t writes[PALLET_TEXT_QUEUE_CAPACITY];
static const uint8_t empty_tile[16] = {0};
static uint8_t write_count;

static uint8_t glyph_tile(char character) {
    if (character >= '0' && character <= '9')
        return TEXT_TILE_BASE + (uint8_t)(character - '0');

    if (character >= 'A' && character <= 'Z')
        return TEXT_TILE_BASE + 10 + (uint8_t)(character - 'A');

    if (character >= 'a' && character <= 'z')
        return TEXT_TILE_BASE + 10 + (uint8_t)(character - 'a');

    switch (character) {
    case '!':
        return TEXT_TILE_BASE + 36;
    case '/':
        return TEXT_TILE_BASE + 37;
    case '-':
        return TEXT_TILE_BASE + 38;
    case '>':
        return TEXT_TILE_BASE + 39;
    default:
        return TEXT_EMPTY_TILE;
    }
}

static void queue_tile(uint8_t x, uint8_t y, uint8_t tile, uint8_t window) {
    if (!window) {
        bg_put(x, y, tile);
        return;
    }

    if (write_count == PALLET_TEXT_QUEUE_CAPACITY)
        return;

    writes[write_count].x = x;
    writes[write_count].y = y;
    writes[write_count].tile = tile;
    write_count++;
}

void text_init(const uint8_t *font_tiles) {
    set_bkg_data(TEXT_EMPTY_TILE, 1, empty_tile);
    set_bkg_data(TEXT_TILE_BASE, TEXT_FONT_TILES, font_tiles);
    set_sprite_data(TEXT_SPRITE_TILE_BASE, TEXT_FONT_TILES, font_tiles);
    set_sprite_data(TEXT_CURSOR_TILE, 1U,
                    font_tiles + (TEXT_FONT_TILES - 1U) * 16U);
    write_count = 0;
}

void text_vblank(void) {
    uint8_t index;

    for (index = 0; index < write_count; index++)
        set_win_tile_xy(writes[index].x, writes[index].y, writes[index].tile);

    write_count = 0;
}

void text_print(uint8_t x, uint8_t y, const char *text) {
    while (*text) {
        queue_tile(x++, y, glyph_tile(*text), 0);
        text++;
    }
}

void text_window_print(uint8_t x, uint8_t y, const char *text) {
    while (*text) {
        queue_tile(x++, y, glyph_tile(*text), 1);
        text++;
    }
}

void text_digits(uint8_t x, uint8_t y, uint16_t value, uint8_t width) {
    uint8_t position = width;

    while (position)
        queue_tile(x + --position, y, TEXT_EMPTY_TILE, 0);

    position = width;
    do {
        if (!position)
            return;
        queue_tile(x + --position, y,
                   TEXT_TILE_BASE + (uint8_t)(value % 10), 0);
        value /= 10;
    } while (value);
}

uint8_t text_sprite_tile(char character) {
    uint8_t tile = glyph_tile(character);

    if (tile == TEXT_EMPTY_TILE)
        return TEXT_EMPTY_TILE;
    return TEXT_SPRITE_TILE_BASE + tile - TEXT_TILE_BASE;
}
