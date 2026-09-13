#ifndef PALLET_DIAGNOSTICS_H
#define PALLET_DIAGNOSTICS_H

#include "status.h"

#include <stdint.h>

/* Debug-build diagnostics for bounded operations that would otherwise
 * fail silently. Release builds (without PALLET_DEBUG) compile these to
 * empty inline stubs, adding no cost.
 *
 * In debug builds modules report failures here; tests and debug builds
 * can read back the last status and per-status occurrence counts. */

#ifdef PALLET_DEBUG
void pallet_diag_report(pallet_status_t status);
pallet_status_t pallet_diag_last(void);
uint8_t pallet_diag_count(pallet_status_t status);
void pallet_diag_reset(void);
#else
static inline void pallet_diag_report(pallet_status_t status) {
    (void)status;
}
static inline pallet_status_t pallet_diag_last(void) {
    return PALLET_OK;
}
static inline uint8_t pallet_diag_count(pallet_status_t status) {
    (void)status;
    return 0;
}
static inline void pallet_diag_reset(void) {
}
#endif

#endif