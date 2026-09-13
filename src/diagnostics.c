#include "diagnostics.h"

#ifdef PALLET_DEBUG

#include <stdint.h>

static pallet_status_t last;
static uint8_t counts[5];

void pallet_diag_report(pallet_status_t status) {
    if (status == PALLET_OK || status > PALLET_BAD_ARGUMENT)
        return;
    last = status;
    if (counts[status] != UINT8_MAX)
        counts[status]++;
}

pallet_status_t pallet_diag_last(void) {
    return last;
}

uint8_t pallet_diag_count(pallet_status_t status) {
    return counts[status];
}

void pallet_diag_reset(void) {
    uint8_t index;

    last = PALLET_OK;
    for (index = 0; index <= PALLET_BAD_ARGUMENT; index++)
        counts[index] = 0;
}

#endif