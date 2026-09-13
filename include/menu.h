#ifndef PALLET_MENU_H
#define PALLET_MENU_H

#include <stdint.h>

#define MENU_NONE 0U
#define MENU_CONFIRM 1U
#define MENU_CANCEL 2U
#define MENU_DENIED 3U

#define MENU_ITEM_DISABLED 0x01U
#define MENU_MAX_ITEMS 16U

/*
 * Open a menu at tile position x, y.  `items` contains `item_count` strings.
 * Opening a menu while another is active makes it that menu's child; up to
 * three menu levels may be open at once.
 */
void menu_open(const char *const *items, uint8_t item_count, uint8_t x,
               uint8_t y);

/*
 * Like menu_open(), with optional flags for each item. `flags` may be NULL.
 * MENU_ITEM_DISABLED leaves an item selectable but makes confirmation return
 * MENU_DENIED. Other flag bits are reserved.
 */
void menu_open_ex(const char *const *items, const uint8_t *flags,
                  uint8_t item_count, uint8_t x, uint8_t y);

/* Process one frame of input and return one of MENU_* result codes. */
uint8_t menu_tick(void);

/* Return the selected item in the active menu, or 0 when none is open. */
uint8_t menu_selected(void);

/* Update an active item's flags, such as MENU_ITEM_DISABLED, and redraw it. */
void menu_set_flag(uint8_t item, uint8_t flags);

/* The active menu owns one pool-allocated cursor slot. */

#endif
