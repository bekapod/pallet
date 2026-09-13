#ifndef PALLET_FLASH_H
#define PALLET_FLASH_H

#include <stdint.h>

extern uint8_t flash_active;

void flash(uint8_t frames);
void flash_tick(void);

#endif
