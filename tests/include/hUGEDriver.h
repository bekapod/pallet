#ifndef TEST_HUGE_H
#define TEST_HUGE_H

/* Minimal fake hUGEDriver for host tests. */
typedef struct {
    int dummy;
} hUGESong_t;

#define HT_CH1 0
#define HT_CH2 1
#define HT_CH3 2
#define HT_CH4 3
#define HT_CH_PLAY 0
#define HT_CH_MUTE 1

extern hUGESong_t test_song;
extern unsigned test_huge_ticks;

void hUGE_init(const hUGESong_t *song);
void hUGE_mute_channel(uint8_t channel, uint8_t mode);
void hUGE_dosound(void);

#endif