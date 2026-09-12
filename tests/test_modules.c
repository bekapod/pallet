#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <gb/gb.h>

#include "blink.h"
#include "fade.h"
#include "input.h"
#include "text.h"

uint8_t BGP_REG;
uint8_t OBP0_REG;
uint8_t OBP1_REG;
uint8_t bkg_tiles[32][32];
uint8_t font_start;
uint8_t font_count;
const void *font_data;

void set_bkg_data(uint8_t first_tile, uint8_t nb_tiles, const void *data) {
    font_start = first_tile;
    font_count = nb_tiles;
    font_data = data;
}

uint8_t *set_bkg_tile_xy(uint8_t x, uint8_t y, uint8_t tile) {
    bkg_tiles[y][x] = tile;
    return &bkg_tiles[y][x];
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
    test_blink();
    test_fade();
    test_text();
    test_input_edges();
    return 0;
}
