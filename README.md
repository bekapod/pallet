# pallet

Pallet is a Game Boy "engine" for GBDK projects.

## Build a browser version

From a game project that includes `engine.mk`, run:

```sh
make web ROM=game
```

The command builds `build/game.gb`, copies the pinned binjgb browser files
into `build/web/`, and creates `build/game-web.zip`.

The keyboard controls are:

- Arrow keys: D-pad
- `X`: A
- `Z`: B
- `Tab`: Select
- `Enter`: Start

## Test the browser build locally

Serve `build/web/` over HTTP. Browsers block the WASM file when you open
`index.html` directly from the filesystem.

```sh
cd build/web
python3 -m http.server 8000
```

Open <http://localhost:8000/> in your browser.
