#ifndef TEST_GBDK_PLATFORM_H
#define TEST_GBDK_PLATFORM_H

/* Host shim for the small part of gbdk/platform.h the engine uses. */
#include <gb/gb.h>

/* CRITICAL { ... } disables interrupts on hardware; a plain scope here. */
#define CRITICAL

#endif