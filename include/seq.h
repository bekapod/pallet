#ifndef PALLET_SEQ_H
#define PALLET_SEQ_H

#include <stdint.h>

#define SEQ_CAPACITY 16

typedef void (*seq_callback_t)(void);

uint8_t seq_push(seq_callback_t callback, uint8_t wait_frames);
void seq_tick(void);
uint8_t seq_busy(void);
void seq_clear(void);

#endif
