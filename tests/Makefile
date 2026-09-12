CC ?= cc
CFLAGS ?= -std=c99 -Wall -Wextra -Werror
TEST_DIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
ROOT := $(abspath $(TEST_DIR)/..)
TEST := $(TEST_DIR)/test_modules
MODULE_SOURCES := $(ROOT)/src/blink.c $(ROOT)/src/fade.c $(ROOT)/src/input.c
MODULE_HEADERS := $(ROOT)/include/blink.h $(ROOT)/include/fade.h $(ROOT)/include/input.h
INCLUDE_FLAGS := -I$(TEST_DIR)/include -I$(ROOT)/include

.PHONY: test clean

test: $(TEST)
	$(TEST)

$(TEST): $(TEST_DIR)/test_modules.c $(MODULE_SOURCES) $(MODULE_HEADERS)
	$(CC) $(CFLAGS) $(INCLUDE_FLAGS) -o $@ $(TEST_DIR)/test_modules.c $(MODULE_SOURCES)

clean:
	rm -f $(TEST)
