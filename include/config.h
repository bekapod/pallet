#ifndef PALLET_CONFIG_H
#define PALLET_CONFIG_H

/* Compile-time knobs. Every value here can be overridden by the consumer
 * with a compiler flag (e.g. -DPALLET_BG_QUEUE_CAPACITY=1024) — no header
 * include-order tricks needed. Values below are the defaults. */

/* Bounded background tile-write queue (entries flushed per VBlank batch). */
#ifndef PALLET_BG_QUEUE_CAPACITY
#define PALLET_BG_QUEUE_CAPACITY 512U
#endif

/* Sprite font/text tile-write queue capacity. */
#ifndef PALLET_TEXT_QUEUE_CAPACITY
#define PALLET_TEXT_QUEUE_CAPACITY 64
#endif

/* OAM slots managed by the sprite pool (hardware allows 40). */
#ifndef PALLET_SPR_SLOTS
#define PALLET_SPR_SLOTS 40U
#endif

/* Maximum number of stacked states (title + overlays). */
#ifndef PALLET_STATE_STACK_CAPACITY
#define PALLET_STATE_STACK_CAPACITY 4U
#endif

/* Maximum rows in one menu. */
#ifndef PALLET_MENU_MAX_ITEMS
#define PALLET_MENU_MAX_ITEMS 16U
#endif

/* Pending callback sequences (fades, delays, staged effects). */
#ifndef PALLET_SEQ_CAPACITY
#define PALLET_SEQ_CAPACITY 16
#endif

#endif