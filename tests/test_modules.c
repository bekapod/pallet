#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <gb/gb.h>

#include "bg.h"
#include "blink.h"
#include "camera.h"
#include "fade.h"
#include "flash.h"
#include "status.h"
#include "fx.h"
#include "hit.h"
#include "input.h"
#include "menu.h"
#include "rng.h"
#include "save.h"
#include "seq.h"
#include "status.h"
#include "shake.h"
#include "scroll.h"
#include "state.h"
#include "spr.h"
#include "text.h"
#include "timer.h"

uint8_t BGP_REG;
uint8_t OBP0_REG;
uint8_t OBP1_REG;
uint8_t LCDC_REG;
uint8_t SCX_REG;
uint8_t SCY_REG;
uint8_t fake_sram[8192];
uint8_t test_ram_enabled;
uint8_t bkg_tiles[32][32];
uint8_t win_tiles[32][32];
uint8_t win_x;
uint8_t win_y;
uint8_t sprite_tiles[40];
uint8_t sprite_x[40];
uint8_t sprite_y[40];
uint8_t sprite_props[40];
uint8_t font_start;
uint8_t font_count;
const void *font_data;
uint8_t sprite_data_start;
uint8_t sprite_data_count;
const uint8_t *sprite_data;
uint8_t sprite_font_start;
uint8_t sprite_font_count;
const uint8_t *sprite_font_data;

void sfx_tick(void) {
}

void set_bkg_data(uint8_t first_tile, uint8_t nb_tiles, const void *data) {
    font_start = first_tile;
    font_count = nb_tiles;
    font_data = data;
}

void set_sprite_data(uint8_t first_tile, uint8_t nb_tiles,
                     const uint8_t *data) {
    if (nb_tiles == TEXT_FONT_TILES) {
        sprite_font_start = first_tile;
        sprite_font_count = nb_tiles;
        sprite_font_data = data;
    } else {
        sprite_data_start = first_tile;
        sprite_data_count = nb_tiles;
        sprite_data = data;
    }
}

uint8_t *set_bkg_tile_xy(uint8_t x, uint8_t y, uint8_t tile) {
    bkg_tiles[y][x] = tile;
    return &bkg_tiles[y][x];
}

void set_bkg_tiles(uint8_t x, uint8_t y, uint8_t w, uint8_t h,
                   const uint8_t *tiles) {
    uint8_t row;
    uint8_t column;

    for (row = 0; row < h; row++) {
        for (column = 0; column < w; column++)
            bkg_tiles[y + row][x + column] = tiles[row * w + column];
    }
}

uint8_t *set_win_tile_xy(uint8_t x, uint8_t y, uint8_t tile) {
    win_tiles[y][x] = tile;
    return &win_tiles[y][x];
}

void move_win(uint8_t x, uint8_t y) {
    win_x = x;
    win_y = y;
}

void set_sprite_tile(uint8_t sprite, uint8_t tile) {
    sprite_tiles[sprite] = tile;
}

void move_sprite(uint8_t sprite, uint8_t x, uint8_t y) {
    sprite_x[sprite] = x;
    sprite_y[sprite] = y;
}

void set_sprite_prop(uint8_t sprite, uint8_t prop) {
    sprite_props[sprite] = prop;
}

enum {
    FONT_DIGIT_0 = 0,
    FONT_LETTER_A = 10,
    FONT_LETTER_Z = 35,
    FONT_EXCLAMATION = 36,
    FONT_SLASH = 37,
    FONT_HYPHEN = 38,
    FONT_GREATER_THAN = 39
};

static uint8_t font_tile(uint8_t offset) {
    return TEXT_TILE_BASE + offset;
}

/* Test code sets this before input_update() to control the sampled buttons. */
static uint8_t joypad_value;

uint8_t joypad(void) {
    return joypad_value;
}

static void test_save(void) {
    uint8_t saved[] = {0x12, 0x34, 0x56};
    uint8_t loaded[sizeof(saved)];
    uint8_t index;

    memset(fake_sram, 0, sizeof(fake_sram));
    test_ram_enabled = 0;

    /* Missing save: typed failure, caller data untouched. */
    memset(loaded, 0xCC, sizeof(loaded));
    assert(save_load(3, loaded, sizeof(loaded)) == SAVE_MISSING);
    for (index = 0; index < sizeof(loaded); index++)
        assert(loaded[index] == 0xCC);
    assert(test_ram_enabled == 0);

    /* Valid save: version owned by the caller's argument, not the data. */
    save_write(3, saved, sizeof(saved));
    assert(test_ram_enabled == 0);
    assert(fake_sram[0] == SAVE_MAGIC);
    assert(fake_sram[1] == 3);
    assert(save_load(3, loaded, sizeof(loaded)) == SAVE_OK);
    assert(memcmp(loaded, saved, sizeof(saved)) == 0);
    assert(test_ram_enabled == 0);

    /* Wrong version: typed failure, caller data untouched. */
    memset(loaded, 0xCC, sizeof(loaded));
    assert(save_load(4, loaded, sizeof(loaded)) == SAVE_BAD_VERSION);
    for (index = 0; index < sizeof(loaded); index++)
        assert(loaded[index] == 0xCC);

    /* Corrupt payload: checksum catches it, caller data untouched. */
    fake_sram[SAVE_HEADER_BYTES] ^= 0xFF;
    assert(save_load(3, loaded, sizeof(loaded)) == SAVE_CORRUPT);
    for (index = 0; index < sizeof(loaded); index++)
        assert(loaded[index] == 0xCC);
    fake_sram[SAVE_HEADER_BYTES] ^= 0xFF;

    /* Zero length is rejected. */
    assert(save_load(3, loaded, 0) == SAVE_BAD_ARGUMENT);
    assert(save_write(3, saved, 0) == SAVE_BAD_ARGUMENT);
    assert(test_ram_enabled == 0);

    /* Wipe makes the next load miss. */
    save_wipe();
    assert(test_ram_enabled == 0);
    assert(save_load(3, loaded, sizeof(loaded)) == SAVE_MISSING);
    assert(test_ram_enabled == 0);
}

static void test_rng(void) {
    rng_t first;
    rng_t second;
    rng_t zero;
    uint16_t first_values[4];
    uint8_t index;

    rng_init(&first, 0x1234);
    rng_init(&second, 0x1234);
    for (index = 0; index < 4; index++) {
        first_values[index] = rng_next(&first);
        assert(first_values[index] == rng_next(&second));
    }

    rng_init(&first, 0x1234);
    rng_init(&second, 0x5678);
    assert(rng_next(&first) == first_values[0]);
    assert(rng_next(&second) != first_values[0]);
    assert(rng_next(&first) == first_values[1]);

    rng_init(&zero, 0);
    assert(rng_next(&zero) != 0);
    assert(rng_range(&zero, 0) == 0);
    for (index = 0; index < 32; index++)
        assert(rng_range(&zero, 17) < 17);

    rng_init(&zero, 1);
    assert(rng_chance(&zero, 0) == 0);
    assert(rng_chance(&zero, 100) == 1);
    assert(rng_chance(&zero, 255) == 1);
}

static void test_blink(void) {
    blink_state_t blink;

    blink_init(&blink);
    assert(blink.frames == BLINK_FRAMES);
    assert(blink.visible == 1);
    assert(blink.dirty == 0);

    /* Count down 29 ticks. The blink should change on the next tick. */
    for (uint8_t frame = 1; frame < BLINK_FRAMES; frame++) {
        blink_tick(&blink);
        assert(blink.frames == BLINK_FRAMES - frame);
        assert(blink.visible == 1);
        assert(blink.dirty == 0);
    }

    blink_tick(&blink);
    assert(blink.frames == BLINK_FRAMES);
    assert(blink.visible == 0);
    assert(blink.dirty == 1);
}

static void test_fade(void) {
    BGP_REG = 0x00;
    OBP0_REG = 0x55;
    OBP1_REG = 0x1B;

    fade_out(2);
    assert(fade_active == 1);
    assert(BGP_REG == 0x00);
    assert(OBP0_REG == 0x55);
    assert(OBP1_REG == 0x1B);

    fade_tick();
    assert(BGP_REG == 0x00);
    fade_tick();
    assert(BGP_REG == 0x55);
    assert(OBP0_REG == 0xAA);
    assert(OBP1_REG == 0x6F);
    fade_tick();
    assert(fade_active == 1);
    fade_tick();
    assert(BGP_REG == 0xAA);
    fade_tick();
    assert(BGP_REG == 0xAA);
    fade_tick();
    assert(BGP_REG == 0xFF);
    assert(OBP0_REG == 0xFF);
    assert(OBP1_REG == 0xFF);
    assert(fade_active == 0);

    fade_in(0);
    assert(fade_active == 1);
    assert(BGP_REG == 0xFF);
    fade_tick();
    assert(BGP_REG == 0xAA);
    fade_tick();
    assert(BGP_REG == 0x55);
    fade_tick();
    assert(BGP_REG == 0x00);
    assert(OBP0_REG == 0x55);
    assert(OBP1_REG == 0x1B);
    assert(fade_active == 0);
}

static unsigned callback_count;
static unsigned callback_order;

static void callback_one(void) {
    callback_count++;
    callback_order = callback_order * 10 + 1;
}

static void callback_two(void) {
    callback_count++;
    callback_order = callback_order * 10 + 2;
}

static void test_sequence(void) {
    seq_clear();
    callback_count = 0;
    callback_order = 0;
    assert(seq_push(callback_one, 2));
    assert(seq_push(0, 1));
    assert(seq_push(callback_two, 0));
    assert(seq_busy());
    seq_tick();
    assert(callback_order == 1);
    seq_tick();
    assert(callback_count == 1);
    seq_tick();
    assert(callback_count == 1);
    seq_tick();
    assert(callback_order == 12);
    assert(!seq_busy());

    for (uint8_t entry = 0; entry < SEQ_CAPACITY; entry++)
        assert(seq_push(0, 1));
    assert(!seq_push(0, 1));
    seq_clear();
    assert(!seq_busy());
}

/* Scroll and shake both want to move the screen. The camera combines
 * them and writes the scroll registers once, so they can't overwrite
 * each other. */
static void test_camera(void) {
    SCX_REG = 9;
    SCY_REG = 8;
    camera_set_y(4);
    camera_apply();
    assert(SCX_REG == 0);
    assert(SCY_REG == 4);

    scroll_set_on_column(0);
    scroll_set_speed(0x0100);
    scroll_reset(0);
    scroll_tick();
    camera_apply();
    assert(SCX_REG == 1);
    assert(SCY_REG == 4);

    /* Shake composes around the current base without resetting it. */
    camera_shake(3, 3);
    camera_tick();
    camera_apply();
    assert(SCX_REG == (uint8_t)(1 + 3) || SCX_REG == (uint8_t)(1 - 3));
    assert(SCY_REG == (uint8_t)(4 + 3) || SCY_REG == (uint8_t)(4 - 3));
    assert(shake_active);

    /* Scrolling continues while shaking. */
    scroll_tick();
    camera_tick();
    camera_apply();
    assert(SCX_REG == (uint8_t)(2 + 3) || SCX_REG == (uint8_t)(2 - 3));

    /* The shake ends on the last duration frame and restores the base. */
    camera_tick();
    camera_apply();
    assert(SCX_REG == 2);
    assert(SCY_REG == 4);
    assert(!shake_active);

    /* Zero-length shake clears offsets immediately. */
    camera_shake(0, 3);
    camera_apply();
    assert(SCX_REG == 2);
    assert(SCY_REG == 4);
}

static void test_shake(void) {
    shake_offset_x = 0;
    shake_offset_y = 0;
    camera_shake(2, 3);
    shake_tick();
    assert(shake_offset_x == 3 || shake_offset_x == (int8_t)-3);
    assert(shake_offset_y == 3 || shake_offset_y == (int8_t)-3);
    assert(shake_active);
    shake_tick();
    assert(shake_offset_x == 0);
    assert(shake_offset_y == 0);
    assert(!shake_active);
}

static void test_flash(void) {
    BGP_REG = 0x12;
    OBP0_REG = 0x34;
    OBP1_REG = 0x56;

    /* Flash fails while a fade owns the palettes. */
    fade_out(2);
    assert(flash(2) == PALLET_BUSY);
    assert(BGP_REG == 0x12);
    assert(flash_active == 0);
    while (fade_active)
        fade_tick();
    assert(!fade_active);

    /* Fade fails while a flash owns the palettes. */
    assert(flash(2) == PALLET_OK);
    assert(fade_out(2) == PALLET_BUSY);
    assert(fade_active == 0);
    while (flash_active)
        flash_tick();
    assert(!flash_active);

    BGP_REG = 0x12;
    OBP0_REG = 0x34;
    OBP1_REG = 0x56;
    assert(flash(2) == PALLET_OK);
    assert(BGP_REG == (uint8_t)~0x12);
    assert(OBP0_REG == (uint8_t)~0x34);
    assert(OBP1_REG == (uint8_t)~0x56);
    flash_tick();
    assert(flash_active);
    flash_tick();
    assert(!flash_active);
    assert(BGP_REG == 0x12);
    assert(OBP0_REG == 0x34);
    assert(OBP1_REG == 0x56);
}

static void test_bg(void) {
    static const uint8_t row_tiles[] = {1, 2, 3};
    static const uint8_t column_tiles[] = {4, 5, 6};
    static const uint8_t long_column_tiles[18] = {0};
    uint8_t frame;

    memset(bkg_tiles, 0xFF, sizeof(bkg_tiles));
    bg_put(0, 0, 7);
    bg_put(1, 0, 8);
    assert(bkg_tiles[0][0] == 0xFF && bkg_tiles[0][1] == 0xFF);
    assert(bg_pending() == 2U);
    bg_flush();
    assert(bkg_tiles[0][0] == 7 && bkg_tiles[0][1] == 8);
    assert(bg_pending() == 0U);

    bg_put_row(31, 4, row_tiles, 3);
    bg_put_col(7, 31, column_tiles, 3);
    assert(bg_pending() == 6U);
    bg_flush();
    assert(bkg_tiles[4][31] == 1 && bkg_tiles[4][0] == 2 &&
           bkg_tiles[4][1] == 3);
    assert(bkg_tiles[31][7] == 4 && bkg_tiles[0][7] == 5 &&
           bkg_tiles[1][7] == 6);
    assert(bg_pending() == 0U);

    bg_put_col(2, 0, long_column_tiles, 18);
    for (frame = 0; frame < 6; frame++)
        bg_put((uint8_t)(10 + frame), 2, 9);
    assert(bg_pending() == 24U);
    bg_flush();
    assert(bg_pending() == 0U);
    assert(bkg_tiles[17][2] == long_column_tiles[17]);
    assert(bkg_tiles[2][15] == 9);

    bg_put_row(0, 6, row_tiles, 3);
    /* A 32-entry row must retain its tail after the first budgeted flush. */
    {
        static uint8_t long_row[32];
        for (frame = 0; frame < 32; frame++)
            long_row[frame] = frame;
        bg_put_row(0, 6, long_row, 32);
        bg_flush();
        assert(bg_pending() == 11U);
        bg_flush();
        assert(bg_pending() == 0U);
        for (frame = 0; frame < 32; frame++)
            assert(bkg_tiles[6][frame] == frame);
    }

    memset(bkg_tiles, 0xFF, sizeof(bkg_tiles));
    bg_fill(0, 0, 20, 18, 11);
    assert(bg_pending() == 360U);
    for (frame = 1; bg_pending(); frame++) {
        bg_flush();
        assert(frame <= 15);
    }
    assert(frame == 16);
    for (uint8_t y = 0; y < 18; y++)
        for (uint8_t x = 0; x < 20; x++)
            assert(bkg_tiles[y][x] == 11);
}

static uint8_t scroll_map_columns[64];
static uint8_t scroll_world_columns[64];
static unsigned scroll_callback_count;

static void record_scroll_column(uint8_t map_col, uint8_t world_col) {
    scroll_map_columns[scroll_callback_count] = map_col;
    scroll_world_columns[scroll_callback_count++] = world_col;
}

static void test_scroll(void) {
    unsigned index;

    memset(scroll_map_columns, 0, sizeof(scroll_map_columns));
    memset(scroll_world_columns, 0, sizeof(scroll_world_columns));
    scroll_callback_count = 0;
    scroll_set_on_column(record_scroll_column);
    scroll_set_speed(0x0100);
    scroll_reset(0);
    assert(scroll_callback_count == 21U);
    for (index = 0; index <= SCROLL_VIEW_COLUMNS; index++) {
        assert(scroll_map_columns[index] == (index & 31U));
        assert(scroll_world_columns[index] == index);
    }
    assert(scroll_x == 0 && scroll_px() == 0 && scroll_col() == 0);
    camera_apply();
    assert(SCX_REG == 0);
    for (index = 0; index < 8; index++)
        scroll_tick();
    assert(scroll_px() == 8 && scroll_col() == 1);
    assert(scroll_callback_count == 22U);
    assert(scroll_map_columns[21] == 21 && scroll_world_columns[21] == 21);

    scroll_callback_count = 0;
    scroll_set_speed(0x0180);
    scroll_reset(0);
    scroll_callback_count = 21U;
    for (index = 0; index < 8; index++)
        scroll_tick();
    assert(scroll_x == 0x0C00 && scroll_px() == 12);
    assert(scroll_world_columns[21] == 21);

    scroll_callback_count = 0;
    scroll_set_speed(0x1900);
    scroll_reset(0);
    scroll_callback_count = 21U;
    scroll_tick();
    assert(scroll_callback_count == 24U);
    assert(scroll_world_columns[21] == 21);
    assert(scroll_world_columns[22] == 22);
    assert(scroll_world_columns[23] == 23);

    scroll_set_speed(0x0100);
    scroll_reset(40);
    scroll_pause();
    {
        uint16_t phase = scroll_x;
        unsigned count = scroll_callback_count;
        for (index = 0; index < 120; index++)
            scroll_tick();
        assert(scroll_x == phase && scroll_callback_count == count);
    }
    scroll_resume();
    scroll_tick();
    assert(scroll_x == (uint16_t)(40U * 8U * 256U + 0x0100U));
    scroll_reset(40);

    assert(scroll_world_to_screen(40U * 8U * 256U) == 0);
    assert(scroll_world_to_screen(40U * 8U * 256U + 159U * 256U) == 159);
    assert(scroll_world_to_screen(40U * 8U * 256U - 256U) == SCROLL_OFFSCREEN);
    assert(scroll_world_to_screen(40U * 8U * 256U + 160U * 256U) == SCROLL_OFFSCREEN);
    assert(scroll_world_to_screen(8U * 256U) == SCROLL_OFFSCREEN);

    scroll_set_on_column(0);
    scroll_reset(0);
    scroll_tick();
}

static void test_text(void) {
    static const uint8_t font[TEXT_FONT_TILES] = {0};

    memset(bkg_tiles, 0xFF, sizeof(bkg_tiles));
    text_init(font);
    assert(font_start == TEXT_TILE_BASE);
    assert(font_count == TEXT_FONT_TILES);
    assert(font_data == font);
    assert(sprite_font_start == TEXT_SPRITE_TILE_BASE);
    assert(sprite_font_count == TEXT_FONT_TILES);
    assert(sprite_font_data == font);
    assert(sprite_data_start == TEXT_CURSOR_TILE);
    assert(sprite_data_count == 1U);
    assert(sprite_data == font + (TEXT_FONT_TILES - 1U) * 16U);
    assert(text_sprite_tile('S') == TEXT_SPRITE_TILE_BASE + 28U);
    assert(text_sprite_tile('>') == TEXT_SPRITE_TILE_BASE + 39U);
    assert(text_sprite_tile(' ') == TEXT_EMPTY_TILE);

    text_print(0, 0, "0aZ!/- >");
    assert(bkg_tiles[0][0] == 0xFF);
    assert(bg_pending() == 8U);
    text_vblank();
    assert(bg_pending() == 8U);
    bg_flush();
    assert(bkg_tiles[0][0] == font_tile(FONT_DIGIT_0));
    assert(bkg_tiles[0][1] == font_tile(FONT_LETTER_A));
    assert(bkg_tiles[0][2] == font_tile(FONT_LETTER_Z));
    assert(bkg_tiles[0][3] == font_tile(FONT_EXCLAMATION));
    assert(bkg_tiles[0][4] == font_tile(FONT_SLASH));
    assert(bkg_tiles[0][5] == font_tile(FONT_HYPHEN));
    assert(bkg_tiles[0][6] == TEXT_EMPTY_TILE);
    assert(bkg_tiles[0][7] == font_tile(FONT_GREATER_THAN));

    text_window_print(4, 5, "A");
    text_vblank();
    assert(win_tiles[5][4] == font_tile(FONT_LETTER_A));

    text_digits(0, 1, 9, 3);
    text_vblank();
    bg_flush();
    assert(bkg_tiles[1][0] == TEXT_EMPTY_TILE);
    assert(bkg_tiles[1][1] == TEXT_EMPTY_TILE);
    assert(bkg_tiles[1][2] == font_tile(9));

    text_digits(0, 2, 65535, 5);
    text_vblank();
    bg_flush();
    assert(bkg_tiles[2][0] == font_tile(6));
    assert(bkg_tiles[2][1] == font_tile(5));
    assert(bkg_tiles[2][2] == font_tile(5));
    assert(bkg_tiles[2][3] == font_tile(3));
    assert(bkg_tiles[2][4] == font_tile(5));

    text_digits(0, 3, 1000, 5);
    text_vblank();
    bg_flush();
    assert(bkg_tiles[3][0] == TEXT_EMPTY_TILE);
    assert(bkg_tiles[3][1] == font_tile(1));
    assert(bkg_tiles[3][2] == font_tile(FONT_DIGIT_0));
    assert(bkg_tiles[3][3] == font_tile(FONT_DIGIT_0));
    assert(bkg_tiles[3][4] == font_tile(FONT_DIGIT_0));
}

static void test_spr(void) {
    uint8_t slot;
    uint8_t index;
    uint8_t old_x;
    uint8_t old_y;
    uint8_t old_tile;
    uint8_t old_prop;

    memset(sprite_x, 0xFF, sizeof(sprite_x));
    memset(sprite_y, 0xFF, sizeof(sprite_y));
    spr_reset();
    for (index = 0; index < SPR_SLOT_COUNT; index++)
        assert(sprite_x[index] == 0 && sprite_y[index] == 0);
    assert(spr_alloc(1) == 0);
    assert(spr_alloc(4) == 1);
    spr_free(1, 4);
    assert(spr_alloc(4) == 1);

    spr_reset();
    assert(spr_alloc(10) == 0);
    assert(spr_alloc(10) == 10);
    assert(spr_alloc(10) == 20);
    spr_free(10, 10);
    assert(spr_alloc(15) == SPR_NONE);
    spr_free(20, 10);
    assert(spr_alloc(15) == 10);

    spr_reset();
    assert(spr_alloc(0) == SPR_NONE);
    assert(spr_alloc(41) == SPR_NONE);
    assert(spr_alloc(40) == 0);
    assert(spr_alloc(1) == SPR_NONE);

    spr_reset();
    slot = spr_alloc(1);
    spr_tile(slot, 23);
    spr_move(slot, 64, 80);
    spr_prop(slot, 7);
    old_x = sprite_x[slot];
    old_y = sprite_y[slot];
    old_tile = sprite_tiles[slot];
    old_prop = sprite_props[slot];
    spr_tile(SPR_NONE, 99);
    spr_move(SPR_NONE, 1, 2);
    spr_prop(SPR_NONE, 99);
    spr_tile(40, 99);
    spr_move(40, 1, 2);
    spr_prop(40, 99);
    spr_free(slot, 1);
    spr_tile(slot, 99);
    spr_move(slot, 1, 2);
    spr_prop(slot, 99);
    assert(sprite_x[slot] == old_x && sprite_y[slot] == old_y);
    assert(sprite_tiles[slot] == old_tile && sprite_props[slot] == old_prop);

    slot = spr_alloc(2);
    spr_move(slot, 12, 34);
    spr_move((uint8_t)(slot + 1), 56, 78);
    spr_free(slot, 0);
    spr_free(SPR_NONE, 1);
    spr_free(slot, 41);
    spr_hide_unused();
    assert(sprite_x[slot] == 12 && sprite_y[slot] == 34);
    assert(sprite_x[slot + 1] == 56 && sprite_y[slot + 1] == 78);
    spr_free(slot, 1);
    spr_hide_unused();
    assert(sprite_x[slot] == 0 && sprite_y[slot] == 0);
    assert(sprite_x[slot + 1] == 56 && sprite_y[slot + 1] == 78);

    spr_reset();
    assert(spr_alloc(40) == 0);
    for (index = 0; index < SPR_SLOT_COUNT; index++)
        assert(sprite_x[index] == 0 && sprite_y[index] == 0);
}

static menu_event_t menu_tick_with(uint8_t buttons) {
    joypad_value = buttons;
    input_update();
    return menu_tick();
}

static void test_menu_navigation_and_flags(void) {
    static const menu_item_t items[] = {
        {"START", 0}, {"SOUND", 0}, {"EXIT", MENU_ITEM_DISABLED}};
    uint8_t frame;

    spr_reset();
    memset(win_tiles, 0xFF, sizeof(win_tiles));
    LCDC_REG = 0;
    menu_open(items, 3, 3, 4);
    text_vblank();
    assert(LCDC_REG & 0x20U);
    assert(win_tiles[0][0] == TEXT_EMPTY_TILE);
    assert(win_tiles[0][1] == font_tile(28)); /* S */
    assert(win_tiles[2][0] == font_tile(FONT_HYPHEN));
    assert(win_x == 31 && win_y == 32);
    assert(sprite_tiles[0] == 0xFEU);
    assert(sprite_x[0] == 32);
    assert(sprite_y[0] == 48);

    assert(menu_tick_with(J_UP).action == MENU_NONE);
    assert(menu_selected() == 2);
    assert(sprite_y[0] == 64);
    assert(menu_tick_with(0).action == MENU_NONE);
    assert(menu_tick_with(J_DOWN).action == MENU_NONE);
    assert(menu_selected() == 0);
    assert(menu_tick_with(0).action == MENU_NONE);
    assert(menu_tick_with(J_DOWN).action == MENU_NONE);
    assert(menu_selected() == 1);
    assert(menu_tick_with(0).action == MENU_NONE);
    assert(menu_tick_with(J_LEFT).action == MENU_NONE);
    assert(menu_selected() == 0);
    for (frame = 0; frame < 14; frame++)
        assert(menu_tick_with(J_LEFT).action == MENU_NONE);
    assert(menu_selected() == 0);
    assert(menu_tick_with(J_LEFT).action == MENU_NONE);
    assert(menu_selected() == 2);
    assert(menu_tick_with(0).action == MENU_NONE);
    assert(menu_tick_with(J_DOWN).action == MENU_NONE);
    assert(menu_selected() == 0);
    for (frame = 0; frame < 14; frame++)
        assert(menu_tick_with(J_DOWN).action == MENU_NONE);
    assert(menu_selected() == 0);
    assert(menu_tick_with(J_DOWN).action == MENU_NONE);
    assert(menu_selected() == 1);
    for (frame = 0; frame < 5; frame++)
        assert(menu_tick_with(J_DOWN).action == MENU_NONE);
    assert(menu_tick_with(J_DOWN).action == MENU_NONE);
    assert(menu_selected() == 2);

    assert(menu_tick_with(0).action == MENU_NONE);
    assert(menu_tick_with(J_A).action == MENU_DENIED);
    menu_set_flag(2, 0);
    text_vblank();
    assert(win_tiles[2][0] == TEXT_EMPTY_TILE);
    assert(menu_tick_with(0).action == MENU_NONE);
    assert(menu_tick_with(J_A).action == MENU_CONFIRM);
    assert(menu_tick_with(0).action == MENU_NONE);
    assert(menu_tick_with(J_B).action == MENU_CANCEL);
    assert(!(LCDC_REG & 0x20U));
    assert(sprite_x[0] == 0 && sprite_y[0] == 0);
}

static void test_menu_parent_stack(void) {
    static const menu_item_t root[] = {{"ROOT", 0}, {"SECOND", 0}};
    static const menu_item_t child[] = {{"CHILD", 0}};
    static const menu_item_t grandchild[] = {{"GRAND", 0}};
    static const menu_item_t rejected[] = {{"REJECTED", 0}};

    spr_reset();
    menu_open(root, 2, 1, 1);
    text_vblank();
    assert(menu_tick_with(J_DOWN).action == MENU_NONE);
    assert(menu_selected() == 1);
    assert(menu_tick_with(0).action == MENU_NONE);

    menu_open(child, 1, 5, 5);
    text_vblank();
    assert(menu_selected() == 0);
    assert(win_x == 47 && win_y == 40);
    assert(win_tiles[1][0] == TEXT_EMPTY_TILE);

    menu_open(grandchild, 1, 7, 7);
    text_vblank();
    assert(win_x == 63 && win_y == 56);
    menu_open(rejected, 1, 9, 9);
    text_vblank();
    assert(win_x == 63 && win_y == 56);
    assert(win_tiles[0][1] == font_tile(16)); /* G */

    assert(menu_tick_with(J_B).action == MENU_CANCEL);
    assert(menu_selected() == 0);
    assert(win_x == 47 && win_y == 40);
    assert(menu_tick_with(0).action == MENU_NONE);
    assert(menu_tick_with(J_B).action == MENU_CANCEL);
    assert(menu_selected() == 1);
    assert(win_x == 15 && win_y == 8);
    assert(menu_tick_with(0).action == MENU_NONE);
    assert(menu_tick_with(J_B).action == MENU_CANCEL);
    assert(!(LCDC_REG & 0x20U));

    menu_open(root, 0, 1, 1);
    assert(!(LCDC_REG & 0x20U));
}

static void test_input_repeat(void) {
    uint8_t frame;

    joypad_value = 0;
    input_update();
    assert(input_repeat(J_RIGHT, 15, 6) == 0);
    assert(input_repeat(J_A, 15, 6) == 0);
    assert(input_repeat(J_RIGHT | J_LEFT, 15, 6) == 0);
    joypad_value = J_RIGHT | J_LEFT;
    input_update();
    assert(input_repeat(J_RIGHT, 15, 6) == 1);
    assert(input_repeat(J_LEFT, 15, 6) == 1);
    for (frame = 0; frame < 14; frame++) {
        input_update();
        assert(input_repeat(J_RIGHT, 15, 6) == 0);
        assert(input_repeat(J_LEFT, 15, 6) == 0);
    }
    input_update();
    assert(input_repeat(J_RIGHT, 15, 6) == 1);
    assert(input_repeat(J_LEFT, 15, 6) == 1);
    joypad_value = 0;
    input_update();
    assert(input_repeat(J_RIGHT, 15, 6) == 0);
    joypad_value = J_RIGHT;
    input_update();
    assert(input_repeat(J_RIGHT, 15, 0) == 1);
    for (frame = 0; frame < 20; frame++) {
        input_update();
        assert(input_repeat(J_RIGHT, 15, 0) == 0);
    }
}

enum {
    STATE_BASE,
    STATE_OVERLAY,
    STATE_REPLACEMENT,
    STATE_OPAQUE,
    STATE_EXTRA
};

static unsigned state_init_count[5];
static unsigned state_update_count[5];
static unsigned state_draw_count[5];
static unsigned state_exit_count[5];
static char state_events[32];
static unsigned state_event_count;

static void state_event(char event, unsigned index) {
    state_events[state_event_count++] = (char)(event + index * 4U);
    if (event == 'i')
        state_init_count[index]++;
    else if (event == 'u')
        state_update_count[index]++;
    else if (event == 'd')
        state_draw_count[index]++;
    else
        state_exit_count[index]++;
}

#define STATE_CALLBACKS(prefix, index) \
    static void prefix##_init(void) { state_event('i', index); } \
    static void prefix##_update(void) { state_event('u', index); } \
    static void prefix##_draw(void) { state_event('d', index); } \
    static void prefix##_exit(void) { state_event('e', index); }

STATE_CALLBACKS(base, STATE_BASE)
STATE_CALLBACKS(overlay, STATE_OVERLAY)
STATE_CALLBACKS(replacement, STATE_REPLACEMENT)
STATE_CALLBACKS(opaque, STATE_OPAQUE)
STATE_CALLBACKS(extra, STATE_EXTRA)

static const state_t base_state = {
    base_init, base_update, base_draw, base_exit, 0
};
static const state_t overlay_state = {
    overlay_init, overlay_update, overlay_draw, overlay_exit, 1
};
static const state_t replacement_state = {
    replacement_init, replacement_update, replacement_draw, replacement_exit, 0
};
static const state_t opaque_state = {
    opaque_init, opaque_update, opaque_draw, opaque_exit, 0
};
static const state_t extra_state = {
    extra_init, extra_update, extra_draw, extra_exit, 1
};

static void state_drain(void) {
    state_pop();
    state_pop();
    state_pop();
    state_pop();
}

static void test_state_stack(void) {
    memset(state_init_count, 0, sizeof(state_init_count));
    memset(state_update_count, 0, sizeof(state_update_count));
    memset(state_draw_count, 0, sizeof(state_draw_count));
    memset(state_exit_count, 0, sizeof(state_exit_count));
    state_event_count = 0;

    state_tick();
    state_pop();

    state_push(&base_state);
    assert(state_init_count[STATE_BASE] == 1);
    state_tick();
    assert(state_update_count[STATE_BASE] == 1);
    assert(state_draw_count[STATE_BASE] == 1);

    state_push(&overlay_state);
    assert(state_init_count[STATE_OVERLAY] == 1);
    state_tick();
    assert(state_update_count[STATE_BASE] == 1);
    assert(state_update_count[STATE_OVERLAY] == 1);
    assert(state_draw_count[STATE_BASE] == 2);
    assert(state_draw_count[STATE_OVERLAY] == 1);
    assert(state_events[state_event_count - 2] ==
           (char)('d' + STATE_BASE * 4U));
    assert(state_events[state_event_count - 1] ==
           (char)('d' + STATE_OVERLAY * 4U));

    state_pop();
    assert(state_exit_count[STATE_OVERLAY] == 1);
    state_tick();
    assert(state_update_count[STATE_BASE] == 2);
    assert(state_draw_count[STATE_BASE] == 3);

    state_replace(&replacement_state);
    assert(state_exit_count[STATE_BASE] == 1);
    assert(state_init_count[STATE_REPLACEMENT] == 1);
    assert(state_events[state_event_count - 2] ==
           (char)('e' + STATE_BASE * 4U));
    assert(state_events[state_event_count - 1] ==
           (char)('i' + STATE_REPLACEMENT * 4U));
    state_tick();
    assert(state_update_count[STATE_REPLACEMENT] == 1);
    assert(state_draw_count[STATE_REPLACEMENT] == 1);
    assert(state_draw_count[STATE_BASE] == 3);

    state_push(&opaque_state);
    state_event_count = 0;
    state_tick();
    assert(state_update_count[STATE_OPAQUE] == 1);
    assert(state_draw_count[STATE_OPAQUE] == 1);
    assert(state_draw_count[STATE_REPLACEMENT] == 1);
    assert(state_event_count == 2);
    state_pop();
    state_pop();

    state_push(&base_state);
    state_replace(0);
    assert(state_init_count[STATE_BASE] == 2);
    state_drain();

    state_push(&base_state);
    state_push(&overlay_state);
    state_push(&replacement_state);
    state_push(&opaque_state);
    state_push(&extra_state);
    assert(state_init_count[STATE_EXTRA] == 0);
    assert(state_exit_count[STATE_OPAQUE] == 1);
    state_pop();
    assert(state_exit_count[STATE_OPAQUE] == 2);
    state_pop();
    assert(state_exit_count[STATE_REPLACEMENT] == 2);
    state_pop();
    assert(state_exit_count[STATE_OVERLAY] == 2);
    state_pop();
    assert(state_exit_count[STATE_BASE] == 3);
    state_pop();

    state_replace(0);
    state_drain();
}

static void test_input_edges(void) {
    uint16_t frame;

    joypad_value = 0;
    input_update();

    /* Start with two buttons down: right and up. */
    joypad_value = J_A | J_B | J_RIGHT | J_UP;
    input_update();
    assert(input_held_frames(J_A) == 1);
    assert(input_held_frames(J_B) == 1);
    assert(input_held_frames(J_A | J_B) == 0);
    assert(input_held(J_A | J_B | J_RIGHT | J_UP));
    assert(input_pressed(J_A) && input_pressed(J_B));
    assert(input_pressed(J_RIGHT) && input_pressed(J_UP));
    assert(!input_pressed(J_START));
    assert(!input_released(J_A));

    /* Keep up held and release right. */
    joypad_value = J_A | J_UP;
    input_update();
    assert(input_held_frames(J_A) == 2);
    assert(input_held_frames(J_B) == 0);
    assert(input_held(J_A | J_UP));
    assert(!input_held(J_B));
    assert(!input_pressed(J_A));
    assert(input_released(J_B) && input_released(J_RIGHT));
    assert(!input_released(J_A));

    /* Release the remaining up button. */
    joypad_value = 0;
    input_update();
    assert(input_held_frames(J_A) == 0);
    assert(input_held_frames(J_B) == 0);
    assert(!input_pressed(J_A));
    assert(input_released(J_A) && input_released(J_UP));

    joypad_value = J_A;
    input_update();
    for (frame = 1; frame < 300; frame++)
        input_update();
    assert(input_held_frames(J_A) == UINT8_MAX);
}

static void test_fixed_point(void) {
    assert(FX(-2) == -512);
    assert(FX_INT(FX(-2)) == -2);
    assert(FX_FRAC(FX(-2) + 0x80) == 128);
    assert(FX_MUL_U8(FX(2), 128) == FX(1));
    assert(FX_SIGN(-1) == -1);
    assert(FX_SIGN(0) == 0);
    assert(FX_SIGN(1) == 1);
    assert(FX_CLAMP(FX(-2), FX(-1), FX(1)) == FX(-1));
    assert(FX_CLAMP(FX(0), FX(-1), FX(1)) == FX(0));
    assert(FX_CLAMP(FX(2), FX(-1), FX(1)) == FX(1));
}

static void test_hit(void) {
    {
        static const hitbox_t a = {0, 0, 10, 10};
        static const hitbox_t b = {9, 0, 10, 10};
        static const hitbox_t c = {10, 0, 10, 10};
        static const hitbox_t d = {20, 0, 10, 10};

        assert(hit_overlaps(a, b));
        assert(!hit_overlaps(a, c));
        assert(!hit_overlaps(a, d));
        assert(hit_contains(a, 9, 9));
        assert(!hit_contains(a, 10, 9));
    }
}

static void test_timer(void) {
    timer_t timer;
    uint8_t frame;

    timer_start(&timer, 60);
    for (frame = 1; frame < 60; frame++)
        assert(timer_tick(&timer) == 0);
    assert(timer_tick(&timer) == 1);
    for (frame = 1; frame < 60; frame++)
        assert(timer_tick(&timer) == 0);
    assert(timer_tick(&timer) == 1);
    timer_start(&timer, 1);
    assert(timer_tick(&timer) == 1);
    assert(timer_tick(&timer) == 1);
    timer_start(&timer, 3);
    timer_tick(&timer);
    timer_reset(&timer);
    assert(timer_tick(&timer) == 0);
    assert(timer_tick(&timer) == 0);
    assert(timer_tick(&timer) == 1);
    timer_start(&timer, 0);
    for (frame = 0; frame < 10; frame++)
        assert(timer_tick(&timer) == 0);
}

int main(void) {
    test_save();
    test_rng();
    test_blink();
    test_fade();
    test_sequence();
    test_camera();
    test_shake();
    test_flash();
    test_bg();
    test_scroll();
    test_text();
    test_spr();
    test_menu_navigation_and_flags();
    test_menu_parent_stack();
    test_input_edges();
    test_input_repeat();
    test_timer();
    test_fixed_point();
    test_hit();
    test_state_stack();
    return 0;
}
