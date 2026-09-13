#ifndef TEST_GB_H
#define TEST_GB_H

#include <stdint.h>

/* These are the GBDK joypad bits used by the host test. */
#define J_RIGHT 0x01U
#define J_LEFT 0x02U
#define J_UP 0x04U
#define J_DOWN 0x08U
#define J_A 0x10U
#define J_B 0x20U
#define J_SELECT 0x40U
#define J_START 0x80U

#define SHOW_WIN (LCDC_REG |= 0x20U)
#define HIDE_WIN (LCDC_REG &= (uint8_t)~0x20U)

/* Test shims for the GBDK interrupt/frame APIs the runtime uses. */
typedef void (*vbl_fn)(void);
void add_VBL(vbl_fn fn);
void vsync(void);

extern uint8_t test_vbl_count;
extern vbl_fn test_vbl_list[8];
extern unsigned test_vsync_count;

extern uint8_t BGP_REG;
extern uint8_t OBP0_REG;
extern uint8_t SCX_REG;
extern uint8_t SCY_REG;
extern uint8_t OBP1_REG;
extern uint8_t LCDC_REG;
extern uint8_t fake_sram[8192];
extern uint8_t test_ram_enabled;

#define ENABLE_RAM (test_ram_enabled = 1)
#define DISABLE_RAM (test_ram_enabled = 0)
#define PALLET_HOST_SRAM ((volatile uint8_t *)fake_sram)

void set_bkg_data(uint8_t first_tile, uint8_t nb_tiles, const void *data);
void set_sprite_data(uint8_t first_tile, uint8_t nb_tiles,
                     const uint8_t *data);
uint8_t *set_bkg_tile_xy(uint8_t x, uint8_t y, uint8_t tile);
void set_bkg_tiles(uint8_t x, uint8_t y, uint8_t w, uint8_t h,
                   const uint8_t *tiles);
uint8_t *set_win_tile_xy(uint8_t x, uint8_t y, uint8_t tile);
void move_win(uint8_t x, uint8_t y);
void set_sprite_tile(uint8_t sprite, uint8_t tile);
void set_sprite_prop(uint8_t sprite, uint8_t prop);
void move_sprite(uint8_t sprite, uint8_t x, uint8_t y);

/* Test-only declaration for the fake joypad supplied by test_modules.c. */
uint8_t joypad(void);

#endif
