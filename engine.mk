ENGINE := $(dir $(lastword $(MAKEFILE_LIST)))
BUILD ?= build
ROM ?= game
GBDK_HOME ?= $(HOME)/gbdk/
LCC := $(GBDK_HOME)/bin/lcc
LCCFLAGS ?= -debug -Wm-yS
GAME_SOURCES := $(wildcard src/*.c)
ENGINE_SOURCES := $(wildcard $(ENGINE)src/*.c)

.PHONY: all
all: $(BUILD)/$(ROM).gb

$(BUILD)/$(ROM).gb: $(GAME_SOURCES) $(ENGINE_SOURCES) $(wildcard include/*.h) | $(BUILD)
	$(LCC) $(LCCFLAGS) -I$(ENGINE)include -o $@ $(GAME_SOURCES) $(ENGINE_SOURCES)

$(BUILD):
	mkdir -p $@

.PHONY: test
test: all
	ROM_NAME=$(ROM) uv run --project tests pytest tests

.PHONY: clean
clean:
	rm -rf $(BUILD)

.DELETE_ON_ERROR:
