#include <gb/gb.h>
#include <stdint.h>

#include "camera.h"
#include "scroll.h"
#include "shake.h"

static uint8_t base_y;

void camera_set_y(uint8_t y) {
    base_y = y;
}

void camera_shake(uint8_t frames, uint8_t amplitude) {
    shake(frames, amplitude);
}

void camera_tick(void) {
    shake_tick();
}

void camera_apply(void) {
    SCX_REG = (uint8_t)(scroll_px() + shake_offset_x);
    SCY_REG = (uint8_t)(base_y + shake_offset_y);
}