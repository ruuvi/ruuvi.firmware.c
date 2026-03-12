#!/bin/bash
# Regenerate compile_commands.json for VS Code IntelliSense.
# Run from repo root: ./scripts/gen_compile_commands.sh
set -e

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
ARMGCC="$REPO_ROOT/src/targets/ruuvitag_b/armgcc"
COMMIT=$(git -C "$REPO_ROOT" rev-parse --short HEAD)
LOG=$(mktemp)

echo "Building ruuvitag_b/default with verbose output..."
make -C "$ARMGCC" clean
make -C "$ARMGCC" \
  "FW_VERSION=-DAPP_FW_VERSION=\\\"${COMMIT}\\\"" \
  DEBUG=-DNDEBUG \
  MODE=-DAPPLICATION_MODE_DEFAULT \
  OPT="-Os -g3" \
  NO_ECHO= 2>&1 | sed "s|'[^']*arm-none-eabi-gcc'|arm-none-eabi-gcc|g" > "$LOG"

echo "Generating compile_commands.json..."
python3 - "$ARMGCC" "$LOG" << 'PYEOF'
import json, re, os, sys

build_dir = sys.argv[1]
entries = []

with open(sys.argv[2]) as f:
    for line in f:
        line = line.rstrip()
        if "arm-none-eabi-gcc" in line and " -c " in line:
            m = re.search(r'-o\s+\S+\s+(\S+\.c)\b', line)
            if m:
                src = m.group(1)
                if not src.startswith("/"):
                    src = os.path.normpath(src)  # keep relative to directory
                entries.append({"directory": build_dir, "command": line.strip(), "file": src})

out = os.path.join(build_dir, "compile_commands.json")
with open(out, "w") as f:
    json.dump(entries, f, indent=2)
print(f"{len(entries)} entries written to {out}")
PYEOF

[[ -f "$LOG" ]] && rm -- "$LOG"
