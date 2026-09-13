#include <gb/gb.h>
#include <stdint.h>

#include "save.h"

#ifdef PALLET_HOST_TEST
#define SRAM_BASE PALLET_HOST_SRAM
#else
#define SRAM_BASE ((volatile uint8_t *)0xA000U)
#endif

uint8_t save_load(void *dst, uint8_t len) {
    uint8_t *bytes = dst;
    uint8_t expected_version;
    uint8_t index;
    volatile uint8_t *sram;

    if (len == 0)
        return 0;

    expected_version = bytes[0];
    ENABLE_RAM;
    sram = SRAM_BASE;
    if (sram[0] == SAVE_MAGIC && sram[1] == expected_version) {
        bytes[0] = expected_version;
        for (index = 1; index < len; index++)
            bytes[index] = sram[index + SAVE_HEADER_BYTES - 1U];
        DISABLE_RAM;
        return 1;
    }
    DISABLE_RAM;
    for (index = 0; index < len; index++)
        bytes[index] = 0;
    return 0;
}

void save_write(const void *src, uint8_t len) {
    const uint8_t *bytes = src;
    uint8_t index;
    volatile uint8_t *sram;

    if (len == 0)
        return;

    ENABLE_RAM;
    sram = SRAM_BASE;
    sram[0] = SAVE_MAGIC;
    sram[1] = bytes[0];
    for (index = 1; index < len; index++)
        sram[index + SAVE_HEADER_BYTES - 1U] = bytes[index];
    DISABLE_RAM;
}

void save_wipe(void) {
    volatile uint8_t *sram;

    ENABLE_RAM;
    sram = SRAM_BASE;
    sram[0] = 0;
    sram[1] = 0;
    DISABLE_RAM;
}
