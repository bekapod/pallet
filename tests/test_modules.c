#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <gb/gb.h>

#include "blink.h"
#include "fade.h"
#include "flash.h"
#include "input.h"
#include "menu.h"
#include "rng.h"
#include "save.h"
#include "seq.h"
#include "shake.h"
#include "text.h"

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
uint8_t font_start;
uint8_t font_count;
const void *font_data;

void sfx_tick(void) {
}

void set_bkg_data(uint8_t first_tile, uint8_t nb_tiles, const void *data) {
    font_start = first_tile;
    font_count = nb_tiles;
    font_data = data;
}

uint8_t *set_bkg_tile_xy(uint8_t x, uint8_t y, uint8_t tile) {
    bkg_tiles[y][x] = tile;
    return &bkg_tiles[y][x];
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
    uint8_t saved[] = {3, 0x12, 0x34, 0x56};
    uint8_t loaded[sizeof(saved)];
    uint8_t index;

    memset(fake_sram, 0, sizeof(fake_sram));
    test_ram_enabled = 0;
    memset(loaded, 0xCC, sizeof(loaded));
    loaded[0] = 3;
    assert(save_load(loaded, sizeof(loaded)) == 0);
    for (index = 0; index < sizeof(loaded); index++)
        assert(loaded[index] == 0);
    assert(test_ram_enabled == 0);

    save_write(saved, sizeof(saved));
    assert(test_ram_enabled == 0);
    assert(fake_sram[0] == SAVE_MAGIC);
    assert(fake_sram[1] == saved[0]);
    loaded[0] = saved[0];
    assert(save_load(loaded, sizeof(loaded)) == 1);
    assert(memcmp(loaded, saved, sizeof(saved)) == 0);
    assert(test_ram_enabled == 0);

    loaded[0] = 4;
    memset(&loaded[1], 0xCC, sizeof(loaded) - 1);
    assert(save_load(loaded, sizeof(loaded)) == 0);
    for (index = 0; index < sizeof(loaded); index++)
        assert(loaded[index] == 0);
    assert(test_ram_enabled == 0);

    save_wipe();
    assert(test_ram_enabled == 0);
    loaded[0] = 3;
    assert(save_load(loaded, sizeof(loaded)) == 0);
    assert(test_ram_enabled == 0);

    fake_sram[0] = SAVE_MAGIC;
    fake_sram[1] = 3;
    save_write(0, 0);
    assert(test_ram_enabled == 0);
    assert(fake_sram[0] == SAVE_MAGIC && fake_sram[1] == 3);
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

static void test_shake(void) {
    SCX_REG = 9;
    SCY_REG = 8;
    shake(2, 3);
    shake_tick();
    assert(SCX_REG == 3);
    assert(SCY_REG == (uint8_t)-3);
    assert(shake_active);
    shake_tick();
    assert(SCX_REG == 0);
    assert(SCY_REG == 0);
    assert(!shake_active);
}

static void test_flash(void) {
    BGP_REG = 0x12;
    OBP0_REG = 0x34;
    OBP1_REG = 0x56;
    flash(2);
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

static void test_text(void) {
    static const uint8_t font[TEXT_FONT_TILES] = {0};

    memset(bkg_tiles, 0xFF, sizeof(bkg_tiles));
    text_init(font);
    assert(font_start == TEXT_TILE_BASE);
    assert(font_count == TEXT_FONT_TILES);
    assert(font_data == font);

    text_print(0, 0, "0aZ!/- >");
    text_vblank();
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
    assert(bkg_tiles[1][0] == TEXT_EMPTY_TILE);
    assert(bkg_tiles[1][1] == TEXT_EMPTY_TILE);
    assert(bkg_tiles[1][2] == font_tile(9));

    text_digits(0, 2, 65535, 5);
    text_vblank();
    assert(bkg_tiles[2][0] == font_tile(6));
    assert(bkg_tiles[2][1] == font_tile(5));
    assert(bkg_tiles[2][2] == font_tile(5));
    assert(bkg_tiles[2][3] == font_tile(3));
    assert(bkg_tiles[2][4] == font_tile(5));

    text_digits(0, 3, 1000, 5);
    text_vblank();
    assert(bkg_tiles[3][0] == TEXT_EMPTY_TILE);
    assert(bkg_tiles[3][1] == font_tile(1));
    assert(bkg_tiles[3][2] == font_tile(FONT_DIGIT_0));
    assert(bkg_tiles[3][3] == font_tile(FONT_DIGIT_0));
    assert(bkg_tiles[3][4] == font_tile(FONT_DIGIT_0));
}

static uint8_t menu_tick_with(uint8_t buttons) {
    joypad_value = buttons;
    input_update();
    return menu_tick();
}

static void test_menu_navigation_and_flags(void) {
    static const char *const items[] = {"START", "SOUND", "EXIT"};
    static const uint8_t flags[] = {0, 0, MENU_ITEM_DISABLED};
    uint8_t frame;

    memset(win_tiles, 0xFF, sizeof(win_tiles));
    LCDC_REG = 0;
    menu_open_ex(items, flags, 3, 3, 4);
    text_vblank();
    assert(LCDC_REG & 0x20U);
    assert(win_tiles[0][0] == TEXT_EMPTY_TILE);
    assert(win_tiles[0][1] == font_tile(28)); /* S */
    assert(win_tiles[2][0] == font_tile(FONT_HYPHEN));
    assert(win_x == 31 && win_y == 32);
    assert(sprite_tiles[0] == font_tile(FONT_GREATER_THAN));
    assert(sprite_x[0] == 32);
    assert(sprite_y[0] == 48);

    assert(menu_tick_with(J_UP) == MENU_NONE);
    assert(menu_selected() == 2);
    assert(sprite_y[0] == 64);
    assert(menu_tick_with(0) == MENU_NONE);
    assert(menu_tick_with(J_DOWN) == MENU_NONE);
    assert(menu_selected() == 0);
    assert(menu_tick_with(0) == MENU_NONE);
    assert(menu_tick_with(J_DOWN) == MENU_NONE);
    assert(menu_selected() == 1);
    for (frame = 0; frame < 14; frame++)
        assert(menu_tick_with(J_DOWN) == MENU_NONE);
    assert(menu_selected() == 1);
    assert(menu_tick_with(J_DOWN) == MENU_NONE);
    assert(menu_selected() == 2);

    assert(menu_tick_with(0) == MENU_NONE);
    assert(menu_tick_with(J_A) == MENU_DENIED);
    menu_set_flag(2, 0);
    text_vblank();
    assert(win_tiles[2][0] == TEXT_EMPTY_TILE);
    assert(menu_tick_with(0) == MENU_NONE);
    assert(menu_tick_with(J_A) == MENU_CONFIRM);
    assert(menu_tick_with(0) == MENU_NONE);
    assert(menu_tick_with(J_B) == MENU_CANCEL);
    assert(!(LCDC_REG & 0x20U));
    assert(sprite_x[0] == 0 && sprite_y[0] == 0);
}

static void test_menu_parent_stack(void) {
    static const char *const root[] = {"ROOT", "SECOND"};
    static const char *const child[] = {"CHILD"};
    static const char *const grandchild[] = {"GRAND"};
    static const char *const rejected[] = {"REJECTED"};

    menu_open(root, 2, 1, 1);
    text_vblank();
    assert(menu_tick_with(J_DOWN) == MENU_NONE);
    assert(menu_selected() == 1);
    assert(menu_tick_with(0) == MENU_NONE);

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

    assert(menu_tick_with(J_B) == MENU_CANCEL);
    assert(menu_selected() == 0);
    assert(win_x == 47 && win_y == 40);
    assert(menu_tick_with(0) == MENU_NONE);
    assert(menu_tick_with(J_B) == MENU_CANCEL);
    assert(menu_selected() == 1);
    assert(win_x == 15 && win_y == 8);
    assert(menu_tick_with(0) == MENU_NONE);
    assert(menu_tick_with(J_B) == MENU_CANCEL);
    assert(!(LCDC_REG & 0x20U));

    menu_open(root, 0, 1, 1);
    assert(!(LCDC_REG & 0x20U));
}

static void test_input_repeat(void) {
    uint8_t frame;

    joypad_value = 0;
    input_update();
    assert(input_repeat(J_RIGHT, 15, 6) == 0);
    joypad_value = J_RIGHT;
    input_update();
    assert(input_repeat(J_RIGHT, 15, 6) == 1);
    for (frame = 0; frame < 14; frame++) {
        input_update();
        assert(input_repeat(J_RIGHT, 15, 6) == 0);
    }
    input_update();
    assert(input_repeat(J_RIGHT, 15, 6) == 1);
    for (frame = 0; frame < 5; frame++) {
        input_update();
        assert(input_repeat(J_RIGHT, 15, 6) == 0);
    }
    input_update();
    assert(input_repeat(J_RIGHT, 15, 6) == 1);
}

static void test_input_edges(void) {
    input_held = 0;
    input_pressed = 0;
    input_released = 0;

    /* Start with two buttons down: right and up. */
    joypad_value = J_RIGHT | J_UP;
    input_update();
    assert(input_held == (J_RIGHT | J_UP));
    assert(input_pressed == (J_RIGHT | J_UP));
    assert(input_released == 0);

    /* Keep up held and release right. */
    joypad_value = J_UP;
    input_update();
    assert(input_held == J_UP);
    assert(input_pressed == 0);
    assert(input_released == J_RIGHT);

    /* Release the remaining up button. */
    joypad_value = 0;
    input_update();
    assert(input_pressed == 0);
    assert(input_released == J_UP);
}

int main(void) {
    test_save();
    test_rng();
    test_blink();
    test_fade();
    test_sequence();
    test_shake();
    test_flash();
    test_text();
    test_menu_navigation_and_flags();
    test_menu_parent_stack();
    test_input_edges();
    test_input_repeat();
    return 0;
}
