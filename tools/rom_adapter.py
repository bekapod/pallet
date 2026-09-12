"""Small PyBoy helpers for symbol-addressed ROM tests."""

import os
import re
from pathlib import Path

ROOT = Path(os.environ.get("ROM_ROOT", Path.cwd()))
ROM_NAME = os.environ.get("ROM_NAME", "game")
ROM = ROOT / "build" / f"{ROM_NAME}.gb"
SYM = ROM.with_suffix(".sym")


def symbol(name: str) -> str:
    return "_" + name


def hook(pyboy, name: str, callback) -> None:
    bank, address = pyboy.symbol_lookup(symbol(name))
    pyboy.hook_register(1 if address >= 0x4000 else bank, address, callback, None)


def unhook(pyboy, name: str) -> None:
    bank, address = pyboy.symbol_lookup(symbol(name))
    pyboy.hook_deregister(1 if address >= 0x4000 else bank, address)


def states() -> dict[str, int]:
    source = (ROOT / "src" / "main.h").read_text()
    body = source.split("enum {", 1)[1].split("};", 1)[0]
    values = {}
    next_value = 0
    for line in body.splitlines():
        name = line.strip().rstrip(",")
        if not name:
            continue
        if "=" in name:
            name, expression = (part.strip() for part in name.split("=", 1))
            next_value = int(expression)
        values[name] = next_value
        next_value += 1
    assert values, "No states found in main.h"
    return values


def eval_defs(pairs, seed=None) -> dict[str, int]:
    values: dict[str, int] = dict(seed or {})
    for name, expression in pairs:
        try:
            values[name] = int(eval(expression, {"__builtins__": {}}, values))
        except NameError:
            continue
    return values


def parse_cdefs(filename: str) -> dict[str, int]:
    source = (ROOT / filename).read_text()
    pairs = re.findall(
        r"(?m)^#define (\w+) +(.+?)\s*(?:(?://|/\*).*)?$", source
    )
    return eval_defs(pairs)
