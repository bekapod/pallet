#include <gb/gb.h>
#include <stdint.h>

#include "save.h"

#ifdef PALLET_HOST_TEST
#define SRAM_BASE PALLET_HOST_SRAM
#else
#define SRAM_BASE ((volatile uint8_t *)0xA000U)
#endif

static uint8_t checksum(const uint8_t *bytes, uint8_t len) {
    uint8_t sum = 0;
    uint8_t index;

    for (index = 0; index < len; index++)
        sum ^= bytes[index];
    return sum;
}

save_result_t save_load(uint8_t version, void *dst, uint8_t len) {
    uint8_t *bytes = dst;
    uint8_t index;
    volatile uint8_t *sram;

    if (len == 0)
        return SAVE_BAD_ARGUMENT;

    ENABLE_RAM;
    sram = SRAM_BASE;
    if (sram[0] != SAVE_MAGIC) {
        DISABLE_RAM;
        return SAVE_MISSING;
    }
    if (sram[1] != version) {
        DISABLE_RAM;
        return SAVE_BAD_VERSION;
    }
    if (sram[2] != checksum((const uint8_t *)sram + SAVE_HEADER_BYTES, len)) {
        DISABLE_RAM;
        return SAVE_CORRUPT;
    }
    for (index = 0; index < len; index++)
        bytes[index] = sram[index + SAVE_HEADER_BYTES];
    DISABLE_RAM;
    return SAVE_OK;
}

save_result_t save_write(uint8_t version, const void *src, uint8_t len) {
    const uint8_t *bytes = src;
    uint8_t index;
    volatile uint8_t *sram;

    if (len == 0)
        return SAVE_BAD_ARGUMENT;

    ENABLE_RAM;
    sram = SRAM_BASE;
    sram[0] = SAVE_MAGIC;
    sram[1] = version;
    sram[2] = checksum(bytes, len);
    for (index = 0; index < len; index++)
        sram[index + SAVE_HEADER_BYTES] = bytes[index];
    DISABLE_RAM;
    return SAVE_OK;
}

void save_wipe(void) {
    volatile uint8_t *sram;

    ENABLE_RAM;
    sram = SRAM_BASE;
    sram[0] = 0;
    DISABLE_RAM;
}