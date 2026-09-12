#ifndef TEST_GB_H
#define TEST_GB_H

#include <stdint.h>

/* These are the GBDK joypad bits used by the host test. */
#define J_RIGHT 0x01U
#define J_UP 0x04U

extern uint8_t BGP_REG;
extern uint8_t OBP0_REG;
extern uint8_t OBP1_REG;

void set_bkg_data(uint8_t first_tile, uint8_t nb_tiles, const void *data);
uint8_t *set_bkg_tile_xy(uint8_t x, uint8_t y, uint8_t tile);

/* Test-only declaration for the fake joypad supplied by test_modules.c. */
uint8_t joypad(void);

#endif
