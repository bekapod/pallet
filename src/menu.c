#include <gb/gb.h>
#include <stdint.h>

#include "input.h"
#include "menu.h"
#include "spr.h"
#include "text.h"

typedef struct {
    const menu_item_t *items;
    uint8_t flags[PALLET_MENU_MAX_ITEMS];
    uint8_t item_count;
    uint8_t x;
    uint8_t y;
    uint8_t selected;
    uint8_t width;
} menu_state_t;

static menu_state_t active_menu;
static menu_state_t parents[2];
static uint8_t menu_depth;
static uint8_t cursor_slot = SPR_NONE;

static uint8_t string_width(const char *text) {
    uint8_t width = 0;

    while (*text++)
        width++;
    return width;
}

static void clear_menu(const menu_state_t *menu) {
    uint8_t row;
    uint8_t column;

    for (row = 0; row < menu->item_count; row++) {
        for (column = 0; column < menu->width + 1U; column++)
            text_window_print(column, row, " ");
    }
}

static void draw_item(uint8_t item) {
    const menu_state_t *menu = &active_menu;

    if (menu->flags[item] & MENU_ITEM_DISABLED)
        text_window_print(0U, item, "-");
    else
        text_window_print(0U, item, " ");
    text_window_print(1U, item, menu->items[item].text);
}

static void move_cursor(void) {
    if (cursor_slot != SPR_NONE) {
        /* Sprite coordinates include the Game Boy's 8x16 hardware offset. */
        spr_move(cursor_slot, (uint8_t)(active_menu.x * 8U + 8U),
                 (uint8_t)((active_menu.y + active_menu.selected) * 8U + 16U));
    }
}

static void draw_menu(void) {
    uint8_t item;

    for (item = 0; item < active_menu.item_count; item++)
        draw_item(item);

    move_win((uint8_t)(active_menu.x * 8U + 7U),
             (uint8_t)(active_menu.y * 8U));
    SHOW_WIN;
    if (cursor_slot != SPR_NONE) {
        spr_tile(cursor_slot, TEXT_CURSOR_TILE);
        spr_prop(cursor_slot, 0U);
        move_cursor();
    }
}

static void hide_menu(void) {
    HIDE_WIN;
    if (cursor_slot != SPR_NONE) {
        spr_move(cursor_slot, 0U, 0U);
        spr_free(cursor_slot, 1U);
        cursor_slot = SPR_NONE;
    }
}

void menu_open(const menu_item_t *items, uint8_t item_count, uint8_t x,
               uint8_t y) {
    menu_state_t next = {0};
    uint8_t item;

    if (!items || !item_count || item_count > PALLET_MENU_MAX_ITEMS)
        return;
    if (menu_depth == 3U)
        return;

    next.items = items;
    next.item_count = item_count;
    next.x = x;
    next.y = y;
    for (item = 0; item < item_count; item++) {
        uint8_t width = string_width(items[item].text);
        next.flags[item] = items[item].flags;
        if (width > next.width)
            next.width = width;
    }

    if (menu_depth) {
        clear_menu(&active_menu);
        parents[menu_depth - 1U] = active_menu;
    } else
        cursor_slot = spr_alloc(1U);
    active_menu = next;
    menu_depth++;
    draw_menu();
}

menu_event_t menu_tick(void) {
    menu_event_t event;
    uint8_t up;
    uint8_t left;
    uint8_t down;
    uint8_t right;

    event.item = 0;
    if (!menu_depth) {
        event.action = MENU_NONE;
        return event;
    }

    up = input_repeat(J_UP, 15U, 6U);
    left = input_repeat(J_LEFT, 15U, 6U);
    down = input_repeat(J_DOWN, 15U, 6U);
    right = input_repeat(J_RIGHT, 15U, 6U);
    if (up || left) {
        if (active_menu.selected)
            active_menu.selected--;
        else
            active_menu.selected = active_menu.item_count - 1U;
        move_cursor();
    } else if (down || right) {
        active_menu.selected++;
        if (active_menu.selected == active_menu.item_count)
            active_menu.selected = 0;
        move_cursor();
    }

    event.item = active_menu.selected;
    if (input_pressed(J_A)) {
        if (active_menu.flags[active_menu.selected] & MENU_ITEM_DISABLED) {
            event.action = MENU_DENIED;
            return event;
        }
        event.action = MENU_CONFIRM;
        return event;
    }

    if (input_pressed(J_B)) {
        clear_menu(&active_menu);
        menu_depth--;
        if (menu_depth) {
            active_menu = parents[menu_depth - 1U];
            draw_menu();
        } else
            hide_menu();
        event.action = MENU_CANCEL;
        return event;
    }

    event.action = MENU_NONE;
    return event;
}

void menu_close(void) {
    if (!menu_depth)
        return;
    clear_menu(&active_menu);
    menu_depth = 0;
    hide_menu();
}

uint8_t menu_selected(void) {
    return menu_depth ? active_menu.selected : MENU_NO_MENU;
}

void menu_set_flag(uint8_t item, uint8_t flags) {
    if (!menu_depth || item >= active_menu.item_count)
        return;

    active_menu.flags[item] = flags;
    draw_item(item);
}
