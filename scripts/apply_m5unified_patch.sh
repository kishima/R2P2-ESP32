#!/bin/bash

# M5Unified component name fix patch
# This script applies a patch to fix component name case sensitivity issue
# in M5Unified CMakeLists.txt for ESP-IDF v5.x compatibility

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
M5UNIFIED_DIR="$PROJECT_ROOT/components/m5unified"

echo "Applying M5Unified patch..."

# Check if M5Unified directory exists
if [ ! -d "$M5UNIFIED_DIR" ]; then
    echo "Error: M5Unified directory not found at $M5UNIFIED_DIR"
    exit 1
fi

# Check if CMakeLists.txt exists
if [ ! -f "$M5UNIFIED_DIR/CMakeLists.txt" ]; then
    echo "Error: CMakeLists.txt not found in $M5UNIFIED_DIR"
    exit 1
fi

# Create backup
cp "$M5UNIFIED_DIR/CMakeLists.txt" "$M5UNIFIED_DIR/CMakeLists.txt.backup"
echo "Created backup: $M5UNIFIED_DIR/CMakeLists.txt.backup"

# Apply patch: Change M5GFX to m5gfx in component requirements
sed -i 's/set(COMPONENT_REQUIRES M5GFX /set(COMPONENT_REQUIRES m5gfx /g' "$M5UNIFIED_DIR/CMakeLists.txt"

echo "Patch applied successfully!"
echo "Changes made:"
echo "  - Changed 'M5GFX' to 'm5gfx' in COMPONENT_REQUIRES"

# Verify the changes
echo ""
echo "Verification:"
grep -n "COMPONENT_REQUIRES.*m5gfx" "$M5UNIFIED_DIR/CMakeLists.txt" || echo "No matches found for verification"

echo ""
echo "Patch application completed."