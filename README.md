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

## Release a version

Release tags are annotated `vMAJOR.MINOR.PATCH` tags on `main`. The workflow
rejects tags in any other format and tags that do not point at a commit on
`main`.

Wait for `main` CI to pass, then run:

```sh
git switch main
git pull --ff-only origin main
git tag -a v0.1.0 -m "v0.1.0"
git push origin v0.1.0
```

The workflow creates a GitHub Release with the commits since the previous tag.
For the first release, it lists the repository history. It also opens a Ditto
PR that updates `engine` to the release commit. Ditto CI builds and tests that
PR.

