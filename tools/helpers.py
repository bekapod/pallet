"""Shared PyBoy harness primitives for ROM tests."""

import pytest

import rom_adapter

ROOT = rom_adapter.ROOT
ROM = rom_adapter.ROM
SYM = rom_adapter.SYM
BOOT_CAP_FRAMES = 600
SETTLE_FRAMES = 8


def wait_for_boot(pyboy, hook_name: str) -> None:
    """Wait until the caller-provided main-loop hook has executed."""
    booted = []
    rom_adapter.hook(pyboy, hook_name, lambda _: booted.append(True))
    for _ in range(BOOT_CAP_FRAMES):
        pyboy.tick(1, render=False)
        if booted:
            rom_adapter.unhook(pyboy, hook_name)
            return
    raise RuntimeError(f"ROM did not reach {hook_name} within {BOOT_CAP_FRAMES} frames")


def addr(gb, name: str) -> int:
    return gb.pyboy.symbol_lookup(rom_adapter.symbol(name))[1]


def parse_header(filename: str) -> dict[str, int]:
    return rom_adapter.parse_cdefs(filename)


def tick_until(gb, condition, cap: int, what: str) -> None:
    for _ in range(cap):
        if condition():
            return
        gb.tick(1)
    pytest.fail(f"timeout ({cap} frames) waiting for {what}")
