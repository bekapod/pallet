CC ?= cc
CFLAGS ?= -std=c99 -Wall -Wextra -Werror
TEST_DIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
ROOT := $(abspath $(TEST_DIR)/..)
TEST := $(TEST_DIR)/test_modules
TEST_RUN := /tmp/pallet-test_modules
MODULES := bg blink fade flash hit input menu rng save scroll seq shake spr state text timer vblank
MODULE_SOURCES := $(addprefix $(ROOT)/src/,$(addsuffix .c,$(MODULES)))
MODULE_HEADERS := $(addprefix $(ROOT)/include/,$(addsuffix .h,$(MODULES))) $(ROOT)/include/fx.h
INCLUDE_FLAGS := -I$(TEST_DIR)/include -I$(ROOT)/include

.PHONY: test clean

test: $(TEST)
	cp $(TEST) $(TEST_RUN)
	chmod +x $(TEST_RUN)
	$(TEST_RUN)

$(TEST): $(TEST_DIR)/test_modules.c $(MODULE_SOURCES) $(MODULE_HEADERS)
	$(CC) $(CFLAGS) -DPALLET_HOST_TEST $(INCLUDE_FLAGS) -o $@ $(TEST_DIR)/test_modules.c $(MODULE_SOURCES)

clean:
	rm -f $(TEST) $(TEST_RUN)
