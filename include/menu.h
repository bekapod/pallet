#ifndef PALLET_MENU_H
#define PALLET_MENU_H

#include <stdint.h>

#define MENU_NONE 0U
#define MENU_CONFIRM 1U
#define MENU_CANCEL 2U
#define MENU_DENIED 3U

/* menu_selected() returns this when no menu is open. */
#define MENU_NO_MENU 0xFFU

#define MENU_ITEM_DISABLED 0x01U
#define MENU_MAX_ITEMS 16U

typedef struct {
    const char *text;
    uint8_t flags;
} menu_item_t;

/* One frame of menu input: the action taken and the item it applies to. */
typedef struct {
    uint8_t action;
    uint8_t item;
} menu_event_t;

/*
 * Open a menu at tile position x, y with the given item descriptors.
 * Opening a menu while another is active makes it that menu's child; up to
 * three menu levels may be open at once. Fails silently on a bad item
 * count or an already-full menu stack.
 */
void menu_open(const menu_item_t *items, uint8_t item_count, uint8_t x,
               uint8_t y);

/* Process one frame of input. D-pad directions move through the items. */
menu_event_t menu_tick(void);

/* Close the active menu and release its cursor. */
void menu_close(void);

/* Return the selected item in the active menu, or MENU_NO_MENU. */
uint8_t menu_selected(void);

/* Update an active item's flags, such as MENU_ITEM_DISABLED, and redraw it. */
void menu_set_flag(uint8_t item, uint8_t flags);

/* The active menu owns one pool-allocated cursor slot. */

#endif