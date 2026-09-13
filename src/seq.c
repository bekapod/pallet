#include <stdint.h>

#include "seq.h"

typedef struct {
    seq_callback_t callback;
    uint8_t wait_frames;
} seq_entry_t;

static seq_entry_t queue[PALLET_SEQ_CAPACITY];
static uint8_t front;
static uint8_t count;
static uint8_t waiting;
static uint8_t started;

static void discard_front(void) {
    front = (front + 1) % PALLET_SEQ_CAPACITY;
    count--;
    started = 0;
}

uint8_t seq_push(seq_callback_t callback, uint8_t wait_frames) {
    uint8_t back;

    if (count == PALLET_SEQ_CAPACITY)
        return 0;
    back = (front + count) % PALLET_SEQ_CAPACITY;
    queue[back].callback = callback;
    queue[back].wait_frames = wait_frames;
    count++;
    return 1;
}

void seq_tick(void) {
    if (!count)
        return;

    if (waiting) {
        waiting--;
        if (waiting)
            return;
        discard_front();
        if (!count)
            return;
    }

    if (!started) {
        if (queue[front].callback)
            queue[front].callback();
        started = 1;
        waiting = queue[front].wait_frames;
        if (waiting)
            return;
        discard_front();
    }
}

uint8_t seq_busy(void) {
    return count != 0;
}

void seq_clear(void) {
    front = 0;
    count = 0;
    waiting = 0;
    started = 0;
}
