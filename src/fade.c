#include <gb/gb.h>
#include <stdint.h>

#include "fade.h"

#include "flash.h"

#define FADE_LEVELS 4
#define FADE_BLACK 3

typedef struct {
    uint8_t original;
} fade_palette_t;

static fade_palette_t bg_palette;
static fade_palette_t obj0_palette;
static fade_palette_t obj1_palette;
static uint8_t fade_level;
static uint8_t fade_frames;
static uint8_t fade_frames_per_step;
static uint8_t fade_direction;
static uint8_t saved_palettes;

uint8_t fade_active;

static uint8_t fade_palette_value(uint8_t palette, uint8_t level) {
    uint8_t result = 0;
    uint8_t entry;

    for (entry = 0; entry < FADE_LEVELS; entry++) {
        uint8_t shade = (palette >> (entry * 2)) & 0x03;
        shade += level;
        if (shade > FADE_BLACK)
            shade = FADE_BLACK;
        result |= shade << (entry * 2);
    }

    return result;
}

static void write_palettes(uint8_t level) {
    BGP_REG = fade_palette_value(bg_palette.original, level);
    OBP0_REG = fade_palette_value(obj0_palette.original, level);
    OBP1_REG = fade_palette_value(obj1_palette.original, level);
}

static void capture_palettes(void) {
    bg_palette.original = BGP_REG;
    obj0_palette.original = OBP0_REG;
    obj1_palette.original = OBP1_REG;
}

static pallet_status_t start_fade(uint8_t frames_per_step, uint8_t direction) {
    if (flash_active)
        return PALLET_BUSY;
    if (!direction || !saved_palettes) {
        capture_palettes();
        saved_palettes = 1;
    }
    fade_frames_per_step = frames_per_step ? frames_per_step : 1;
    fade_frames = fade_frames_per_step;
    fade_direction = direction;
    fade_level = direction ? FADE_BLACK : 0;
    write_palettes(fade_level);
    fade_active = 1;
    return PALLET_OK;
}

pallet_status_t fade_out(uint8_t frames_per_step) {
    saved_palettes = 0;
    return start_fade(frames_per_step, 0);
}

pallet_status_t fade_in(uint8_t frames_per_step) {
    return start_fade(frames_per_step, 1);
}

void fade_tick(void) {
    if (!fade_active)
        return;

    if (--fade_frames)
        return;

    fade_frames = fade_frames_per_step;
    if (fade_direction) {
        if (fade_level == 0) {
            fade_active = 0;
            return;
        }
        fade_level--;
    } else {
        if (fade_level == FADE_BLACK) {
            fade_active = 0;
            return;
        }
        fade_level++;
    }

    write_palettes(fade_level);
    if ((!fade_direction && fade_level == FADE_BLACK) ||
        (fade_direction && fade_level == 0))
        fade_active = 0;
}
