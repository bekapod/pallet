#include <assert.h>
#include <stdint.h>
#include <gb/gb.h>

#include "blink.h"
#include "fade.h"
#include "input.h"

uint8_t BGP_REG;
uint8_t OBP0_REG;
uint8_t OBP1_REG;

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
    test_input_edges();
    return 0;
}
