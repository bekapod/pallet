#ifndef PALLET_SFX_H
#define PALLET_SFX_H

#include <stdint.h>

#define SFX_END 0xFF
#define SFX_TRIGGER 0x80

#define SFX_REG_PULSE2_DUTY 0x16
#define SFX_REG_PULSE2_ENV 0x17
#define SFX_REG_PULSE2_FREQ_LO 0x18
#define SFX_REG_PULSE2_FREQ_HI 0x19
#define SFX_REG_NOISE_ENV 0x21
#define SFX_REG_NOISE_PITCH 0x22
#define SFX_REG_NOISE_TRIGGER 0x23

typedef struct {
    uint8_t delay;
    uint8_t reg;
    uint8_t value;
} sfx_row_t;

typedef enum {
    SFX_CHANNEL_2,
    SFX_CHANNEL_4
} sfx_channel_t;

typedef struct {
    const sfx_row_t *rows;
    sfx_channel_t channel;
} sfx_script_t;

#define SFX_DUTY_12_5 0x00
#define SFX_ENV(vol, fade) (((vol) << 4) | (fade))
#define SFX_NOISE(shift, div) (((shift) << 4) | (div))
#define SFX_PITCH_LO(pitch) ((pitch) & 0xFF)
#define SFX_PITCH_HI(pitch) ((pitch) >> 8)

void sfx_init(void);
void sfx_play(const sfx_script_t *script);
void sfx_tick(void);

#endif
