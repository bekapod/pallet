#ifndef PALLET_RUNTIME_H
#define PALLET_RUNTIME_H

#include <stdint.h>

#include "hUGEDriver.h"
#include "state.h"

/* Consumer boot configuration. main() declares one of these and calls
 * pallet_run(); Pallet owns everything else about the frame loop. */
typedef struct {
    /* First state pushed at boot; required. */
    const state_t *initial_state;

    /* Optional hUGE song started at boot. Zero plays nothing. */
    const hUGESong_t *music;

    /* Optional advanced hook. Pallet calls it from its VBlank ISR after
     * audio, fades, and camera/palette register writes. Keep it short —
     * everything here must fit inside the VBlank budget alongside
     * Pallet's own work. Leave zero unless you need raw VBlank work. */
    void (*on_vblank)(void);
} pallet_game_t;

/* Initialize modules, register VBlank work, start music, and push the
 * initial state. Exposed mainly for tests; pallet_run() calls it. */
void pallet_boot(const pallet_game_t *game);

/* One engine frame: sample input, update and draw the active state,
 * advance sequences, hide unused sprites, sync, flush queued writes.
 * Exposed for tests; pallet_run() loops it. */
void pallet_frame(void);

/* Pallet's VBlank ISR: audio/fade/camera/flash state, then the optional
 * consumer hook. Registered by pallet_boot(). */
void pallet_vblank_isr(void);

/* Boot, then run the engine frame loop forever. */
void pallet_run(const pallet_game_t *game);

#endif