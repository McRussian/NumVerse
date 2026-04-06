#!/bin/bash

BUILD_DIR="$(dirname "$0")/build/tests"
PASS=0
FAIL=0
FAILED_BINS=()

if [ ! -d "$BUILD_DIR" ]; then
    echo "Build directory not found: $BUILD_DIR"
    echo "Run: cmake --build build"
    exit 1
fi

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
