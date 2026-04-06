#!/bin/bash

ROOT_DIR="$(dirname "$0")"
BUILD_DIR="$ROOT_DIR/build/tests"
PASS=0
FAIL=0
FAILED_BINS=()

cmake -B "$ROOT_DIR/build" -S "$ROOT_DIR" -DCMAKE_BUILD_TYPE=Debug 2>&1 | tail -2
cmake --build "$ROOT_DIR/build" 2>&1 | tail -5
echo ""

for bin in "$BUILD_DIR"/test_*; do
    [ -f "$bin" ] && [ -x "$bin" ] || continue
    echo "=== $(basename "$bin") ==="
    "$bin"
    if [ $? -eq 0 ]; then
        PASS=$((PASS + 1))
    else
        FAIL=$((FAIL + 1))
        FAILED_BINS+=("$(basename "$bin")")
    fi
    echo ""
done

echo "=============================="
echo "Binaries passed: $PASS"
if [ $FAIL -gt 0 ]; then
    echo "Binaries failed: $FAIL"
    for b in "${FAILED_BINS[@]}"; do
        echo "  FAILED: $b"
    done
    exit 1
else
    echo "All tests passed."
fi
