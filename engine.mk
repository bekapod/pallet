ENGINE := $(dir $(lastword $(MAKEFILE_LIST)))
BUILD ?= build
ROM ?= game
GBDK_HOME ?= $(HOME)/gbdk/
LCC := $(GBDK_HOME)/bin/lcc
PNG2ASSET := $(GBDK_HOME)/bin/png2asset
TOOLS_DIR ?= $(CURDIR)/.tools
HUGETRACKER_VERSION := 1.0.11
HUGETRACKER_PLATFORM := linux
HUGETRACKER_SHA256 := 9e2e21d50d2ebbeb5653168c483279b188ff1b01b908b685172b0b353e506e24
ifeq ($(shell uname -s),Darwin)
HUGETRACKER_PLATFORM := mac
HUGETRACKER_SHA256 := 259a694fd93ae5f6c430e13ca7fcca0c86c83b7b2bfd50c38394cbdbd5f8a5d0
endif
HUGETRACKER_URL := https://github.com/SuperDisk/hUGETracker/releases/download/v$(HUGETRACKER_VERSION)/hUGETracker-$(HUGETRACKER_VERSION)-$(HUGETRACKER_PLATFORM).zip
UGE2SOURCE ?= $(TOOLS_DIR)/uge2source
LCCFLAGS ?= -debug -Wl-j -Wm-yS -Wl-yt1 -Wl-yo2 -Wl-ya0
HUGE_DRIVER_LIB := $(ENGINE)third_party/hUGEDriver/gbdk/hUGEDriver.lib
GAME_SOURCES := $(wildcard src/*.c)
GAME_HEADERS := $(wildcard src/*.h)
ENGINE_SOURCES := $(wildcard $(ENGINE)src/*.c)
ENGINE_HEADERS := $(wildcard $(ENGINE)include/*.h)

MAP_PNGS := $(wildcard art/*_map.png)
FULLSCREEN_PNGS := $(wildcard art/*_screen.png)
SHEET_PNGS := $(filter-out $(MAP_PNGS) $(FULLSCREEN_PNGS),$(wildcard art/*.png))
RESOURCE_SOURCES := $(patsubst art/%.png,res/%.c,$(wildcard art/*.png))
MUSIC_SOURCES := $(patsubst music/%.uge,res/%.c,$(wildcard music/*.uge))

.PHONY: all setup
all: $(BUILD)/$(ROM).gb

setup: $(UGE2SOURCE)

$(UGE2SOURCE):
	mkdir -p $(TOOLS_DIR)
	curl -fsSL -o $(TOOLS_DIR)/hugetracker.zip $(HUGETRACKER_URL)
	echo "$(HUGETRACKER_SHA256)  $(TOOLS_DIR)/hugetracker.zip" | shasum -a 256 -c
	unzip -q -o $(TOOLS_DIR)/hugetracker.zip uge2source -d $(TOOLS_DIR)
	chmod +x $@
	rm $(TOOLS_DIR)/hugetracker.zip

$(BUILD)/$(ROM).gb: $(GAME_SOURCES) $(GAME_HEADERS) $(ENGINE_SOURCES) $(ENGINE_HEADERS) $(RESOURCE_SOURCES) $(MUSIC_SOURCES) | $(BUILD)
	$(LCC) $(LCCFLAGS) -I$(ENGINE)include -I$(ENGINE)third_party/hUGEDriver/include -Wl-l$(HUGE_DRIVER_LIB) -o $@ $(GAME_SOURCES) $(ENGINE_SOURCES) $(RESOURCE_SOURCES) $(MUSIC_SOURCES)

$(patsubst art/%.png,res/%.c,$(MAP_PNGS)): res/%.c: art/%.png art/tileset.png | res
	$(PNG2ASSET) $< -o $@ -map -source_tileset art/tileset.png -noflip -keep_palette_order -no_palettes > $@.log 2>&1 || { cat $@.log; false; }
	! grep -q "not in the source tileset" $@.log || { cat $@.log; false; }

$(patsubst art/%.png,res/%.c,$(SHEET_PNGS)): res/%.c: art/%.png | res
	$(PNG2ASSET) $< -o $@ -map -tiles_only -keep_duplicate_tiles -noflip -keep_palette_order -no_palettes

$(patsubst art/%.png,res/%.c,$(FULLSCREEN_PNGS)): res/%.c: art/%.png | res
	$(PNG2ASSET) $< -o $@ -map -noflip -keep_palette_order -no_palettes

$(MUSIC_SOURCES): res/%.c: music/%.uge | res
	$(UGE2SOURCE) $< $* $@

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
