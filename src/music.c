#include <gbdk/platform.h>

#include "music.h"

void music_play(const hUGESong_t *song) {
    CRITICAL {
        hUGE_init(song);
        hUGE_mute_channel(HT_CH1, HT_CH_PLAY);
        hUGE_mute_channel(HT_CH2, HT_CH_MUTE);
        hUGE_mute_channel(HT_CH3, HT_CH_PLAY);
        hUGE_mute_channel(HT_CH4, HT_CH_MUTE);
    }
}

void music_stop(void) {
    CRITICAL {
        hUGE_mute_channel(HT_CH1, HT_CH_MUTE);
        hUGE_mute_channel(HT_CH3, HT_CH_MUTE);
    }
}
