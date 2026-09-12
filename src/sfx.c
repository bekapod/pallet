#include <gbdk/platform.h>
#include <stdint.h>

#include "sfx.h"

typedef struct {
    const sfx_row_t *row;
    uint8_t delay;
} sfx_channel_state_t;

static sfx_channel_state_t channel2;
static sfx_channel_state_t channel4;

static sfx_channel_state_t *channel_state(sfx_channel_t channel) {
    return channel == SFX_CHANNEL_2 ? &channel2 : &channel4;
}

void sfx_init(void) {
    NR52_REG = 0x80;
    NR50_REG = 0x77;
    NR51_REG = 0xAA;
    channel2.row = 0;
    channel4.row = 0;
}

void sfx_play(const sfx_script_t *script) {
    sfx_channel_state_t *channel = channel_state(script->channel);

    CRITICAL {
        channel->row = script->rows;
        channel->delay = channel->row->delay;
    }
}

static void tick_channel(sfx_channel_state_t *channel) {
    if (!channel->row)
        return;

    if (channel->delay) {
        channel->delay--;
        return;
    }

    for (;;) {
        *(volatile uint8_t *)(0xFF00 + channel->row->reg) = channel->row->value;
        channel->row++;

        if (channel->row->delay == SFX_END) {
            channel->row = 0;
            return;
        }

        channel->delay = channel->row->delay;
        if (channel->delay)
            return;
    }
}

void sfx_tick(void) {
    tick_channel(&channel2);
    tick_channel(&channel4);
}
