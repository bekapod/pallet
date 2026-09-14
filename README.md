# Pallet

Pallet is a small Game Boy runtime for GBDK projects. It owns the frame loop
and shared hardware protocols. Your game owns its rules, data, and state
modules.

## Start a game

Declare the initial state and call `pallet_run()` from `main()`:

```c
#include <gb/gb.h>
#include "pallet.h"

static void title_update(void);
static const state_t title = {0, title_update, 0, 0, 0};
static const pallet_game_t game = {&title, 0, 0};

static void title_update(void) {
	if (input_pressed(J_START))
		(void)state_replace(&title);
}

void main(void) {
	text_init(font_tiles);
	SHOW_BKG;
	SHOW_SPRITES;
	pallet_run(&game);
}
```

State descriptors have static lifetime. State callbacks take no arguments. Keep
one private context struct in each state module when a state needs mutable data.
Use `init` to acquire resources, `update` for game rules, `draw` for
presentation, and `exit` to release resources.

## Ownership

Pallet owns:

- input sampling and button queries;
- state dispatch and deferred transitions;
- VBlank timing;
- camera and palette register composition;
- sprite allocation;
- bounded background and text queues;
- generic sequences;
- save framing and checksums;
- music and sound-effect scheduling.

The game owns state rules, actors, health, collision meaning, levels, tracks,
weapons, enemies, inventory, progression, and save payload schemas. Pallet
provides rectangles and storage.

## Frame lifecycle

`pallet_run()` initializes Pallet, starts the optional song, and pushes the
initial state. Each frame follows this order:

1. Sample input.
2. Update the active state.
3. Apply deferred state transitions.
4. Draw the visible state stack.
5. Advance sequences.
6. Wait for VBlank.
7. Run Pallet audio and effect work.
8. Call the optional VBlank hook.
9. Compose camera and palette registers.
10. Flush queued background and text writes.

Pallet registers its own VBlank work, so the game's `main()` ends at
`pallet_run()`.

## Units and results

Scroll positions and world positions use unsigned 8.8 fixed-point pixels. Use
`SCROLL_SPEED_PX(1)` instead of a raw `0x0100`. Camera positions and shake
offsets use pixels. Hitboxes are half-open pixel rectangles. Query button edges
with calls such as `input_pressed(J_A)`.

Bounded operations return a `pallet_status_t`. `PALLET_FULL` means that a
capacity limit was reached. `PALLET_EMPTY` means that there was nothing to
remove. `PALLET_BUSY` means that another effect owns the resource.

`save_load(version, dst, len)` leaves `dst` unchanged when the save is missing,
has the wrong version, or fails its checksum.

## Advanced interfaces

Use these interfaces when the game needs hardware control beyond the standard
runtime:

- Raw OAM operations support custom sprite layouts.
- `pallet_game_t.on_vblank` runs a short consumer callback during VBlank. Pallet
  still owns camera and palette register writes.
- `PALLET_*` compiler flags override capacities. Put these flags in the game's
  build configuration.
- Direct queue flushing and other hardware calls are limited to their documented
  frame phases.

## Build a browser package

From a project that includes `engine.mk`, run:

```sh
make web ROM=game
```

The command writes `build/game.gb`, copies the pinned binjgb files into
`build/web/`, and creates `build/game-web.zip`. Serve `build/web/` over HTTP.
Browsers block the WASM file when you open `index.html` from the filesystem.

The browser controls are:

- Arrow keys: D-pad
- `X`: A
- `Z`: B
- `Tab`: Select
- `Enter`: Start

## Release Pallet

Release tags use the form `vMAJOR.MINOR.PATCH` and must point to a commit on
`main`. After CI passes, run:

```sh
git switch main
git pull --ff-only origin main
git tag -a v0.1.0 -m "v0.1.0"
git push origin v0.1.0
```
