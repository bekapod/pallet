#include "fade.h"
#include "flash.h"
#include "camera.h"
#include "sfx.h"
#include "scroll.h"
#include "vblank.h"

void pallet_vblank_tick(void) {
    sfx_tick();
    fade_tick();
    camera_tick();
    camera_apply();
    flash_tick();
}