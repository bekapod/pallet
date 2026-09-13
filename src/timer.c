#include <stdint.h>

#include "timer.h"

void timer_start(timer_t *timer, uint8_t period) {
    timer->period = period;
    timer->count = period;
}

uint8_t timer_tick(timer_t *timer) {
    if (!timer->period)
        return 0;

    if (--timer->count == 0U) {
        timer->count = timer->period;
        return 1;
    }
    return 0;
}

void timer_reset(timer_t *timer) {
    timer->count = timer->period;
}
