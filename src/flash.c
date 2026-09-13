#include <gb/gb.h>
#include <stdint.h>

#include "flash.h"

static uint8_t frames;
static uint8_t bg_palette;
static uint8_t obj0_palette;
static uint8_t obj1_palette;

uint8_t flash_active;

void flash(uint8_t duration) {
    bg_palette = BGP_REG;
    obj0_palette = OBP0_REG;
    obj1_palette = OBP1_REG;
    frames = duration;
    if (!duration) {
        flash_active = 0;
        return;
    }

    BGP_REG = (uint8_t)~bg_palette;
    OBP0_REG = (uint8_t)~obj0_palette;
    OBP1_REG = (uint8_t)~obj1_palette;
    flash_active = 1;
}

void flash_tick(void) {
    if (!flash_active)
        return;
    if (--frames)
        return;

    BGP_REG = bg_palette;
    OBP0_REG = obj0_palette;
    OBP1_REG = obj1_palette;
    flash_active = 0;
}
