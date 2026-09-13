#include <gb/gb.h>
#include <gbdk/platform.h>
#include <stdint.h>

#include "bg.h"
#include "camera.h"
#include "fade.h"
#include "flash.h"
#include "input.h"
#include "music.h"
#include "runtime.h"
#include "scroll.h"
#include "seq.h"
#include "sfx.h"
#include "spr.h"
#include "state.h"
#include "text.h"
#include "vblank.h"

static const pallet_game_t *boot_game;

void pallet_vblank_isr(void) {
    /* Pallet owns the hardware composition order; the consumer hook runs
     * last and must stay within the remaining VBlank budget. */
    pallet_vblank_tick();
    if (boot_game && boot_game->on_vblank)
        boot_game->on_vblank();
}

void pallet_boot(const pallet_game_t *game) {
    boot_game = game;
    sfx_init();
    add_VBL(pallet_vblank_isr);
    if (game->music) {
        music_play(game->music);
        add_VBL(hUGE_dosound);
    }
    if (game->initial_state)
        state_push(game->initial_state);
}

void pallet_frame(void) {
    input_update();
    state_tick();
    spr_hide_unused();
    seq_tick();
    vsync();
    text_vblank();
    bg_flush();
}

void pallet_run(const pallet_game_t *game) {
    pallet_boot(game);
    for (;;)
        pallet_frame();
}