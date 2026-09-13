#include "fade.h"
#include "flash.h"
#include "sfx.h"
#include "shake.h"
#include "vblank.h"

void pallet_vblank_tick(void) {
    sfx_tick();
    fade_tick();
    shake_tick();
    flash_tick();
}
