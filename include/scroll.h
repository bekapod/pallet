#ifndef SCROLL_H
#define SCROLL_H

#include <stdint.h>

#define SCROLL_VIEW_COLUMNS 20U
#define SCROLL_LOOKAHEAD_COLUMNS 20U
#define SCROLL_OFFSCREEN 0xFFU

typedef void (*scroll_column_callback_t)(uint8_t map_col, uint8_t world_col);

/* The hardware phase wraps every 256 pixels. */
extern uint16_t scroll_x;

void scroll_set_on_column(scroll_column_callback_t callback);
void scroll_set_speed(uint16_t speed);
void scroll_tick(void);
uint8_t scroll_px(void);
uint8_t scroll_col(void);
void scroll_reset(uint8_t world_col);
void scroll_pause(void);
void scroll_resume(void);
uint8_t scroll_world_to_screen(uint32_t world_x_fx);

/* Callback tile storage must remain valid until bg_pending() reaches zero.
 * Callback arguments are wrapped modulo 256; map_col is also limited to 32. */

#endif
