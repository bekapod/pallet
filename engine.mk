ENGINE := $(dir $(lastword $(MAKEFILE_LIST)))
BUILD ?= build
ROM ?= game
GBDK_HOME ?= $(HOME)/gbdk/
LCC := $(GBDK_HOME)/bin/lcc
PNG2ASSET := $(GBDK_HOME)/bin/png2asset
LCCFLAGS ?= -debug -Wl-j -Wm-yS -Wl-yt1 -Wl-yo2 -Wl-ya0
GAME_SOURCES := $(wildcard src/*.c)
GAME_HEADERS := $(wildcard src/*.h)
ENGINE_SOURCES := $(wildcard $(ENGINE)src/*.c)
ENGINE_HEADERS := $(wildcard $(ENGINE)include/*.h)

MAP_PNGS := $(wildcard art/*_map.png)
FULLSCREEN_PNGS := $(wildcard art/*_screen.png)
SHEET_PNGS := $(filter-out $(MAP_PNGS) $(FULLSCREEN_PNGS),$(wildcard art/*.png))
RESOURCE_SOURCES := $(patsubst art/%.png,res/%.c,$(wildcard art/*.png))

.PHONY: all
all: $(BUILD)/$(ROM).gb

$(BUILD)/$(ROM).gb: $(GAME_SOURCES) $(GAME_HEADERS) $(ENGINE_SOURCES) $(ENGINE_HEADERS) $(RESOURCE_SOURCES) | $(BUILD)
	$(LCC) $(LCCFLAGS) -I$(ENGINE)include -o $@ $(GAME_SOURCES) $(ENGINE_SOURCES) $(RESOURCE_SOURCES)

$(patsubst art/%.png,res/%.c,$(MAP_PNGS)): res/%.c: art/%.png art/tileset.png | res
	$(PNG2ASSET) $< -o $@ -map -source_tileset art/tileset.png -noflip -keep_palette_order -no_palettes > $@.log 2>&1 || { cat $@.log; false; }
	! grep -q "not in the source tileset" $@.log || { cat $@.log; false; }

$(patsubst art/%.png,res/%.c,$(SHEET_PNGS)): res/%.c: art/%.png | res
	$(PNG2ASSET) $< -o $@ -map -tiles_only -keep_duplicate_tiles -noflip -keep_palette_order -no_palettes

$(patsubst art/%.png,res/%.c,$(FULLSCREEN_PNGS)): res/%.c: art/%.png | res
	$(PNG2ASSET) $< -o $@ -map -noflip -keep_palette_order -no_palettes

res:
	mkdir -p $@

$(BUILD):
	mkdir -p $@

.PHONY: test
test: all
	ROM_NAME=$(ROM) uv run --project tests pytest tests

.PHONY: clean
clean:
	rm -rf $(BUILD) res

.DELETE_ON_ERROR:
