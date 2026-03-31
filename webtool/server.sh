#!/bin/bash

# R2P2-ESP32 Web Flasher Local Server
# This script starts a simple HTTP server to serve the web flasher

PORT=${1:-8000}
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

echo "============================================"
echo "  R2P2-ESP32 Web Flasher Server"
echo "  install webrick if not already installed" 
echo "============================================"
echo ""

# Check if build directory exists
if [ ! -d "$PROJECT_ROOT/build" ]; then
    echo "Error: Build directory not found!"
    echo "Please run 'rake build' first."
    echo ""
    exit 1
fi

# Copy firmware files to webtool/firmware directory
echo "Copying firmware files..."
mkdir -p "$SCRIPT_DIR/firmware"

if [ -f "$PROJECT_ROOT/build/bootloader/bootloader.bin" ]; then
    cp "$PROJECT_ROOT/build/bootloader/bootloader.bin" "$SCRIPT_DIR/firmware/"
    echo "  ✓ bootloader.bin"
else
    echo "  ✗ bootloader.bin not found"
fi

if [ -f "$PROJECT_ROOT/build/partition_table/partition-table.bin" ]; then
    cp "$PROJECT_ROOT/build/partition_table/partition-table.bin" "$SCRIPT_DIR/firmware/"
    echo "  ✓ partition-table.bin"
else
    echo "  ✗ partition-table.bin not found"
fi

if [ -f "$PROJECT_ROOT/build/R2P2-ESP32.bin" ]; then
    cp "$PROJECT_ROOT/build/R2P2-ESP32.bin" "$SCRIPT_DIR/firmware/"
    echo "  ✓ R2P2-ESP32.bin"
else
    echo "  ✗ R2P2-ESP32.bin not found"
fi

if [ -f "$PROJECT_ROOT/build/storage.bin" ]; then
    cp "$PROJECT_ROOT/build/storage.bin" "$SCRIPT_DIR/firmware/"
    echo "  ✓ storage.bin"
else
    echo "  ✗ storage.bin not found"
fi

echo ""
echo "Starting HTTP server on port $PORT..."
echo ""
echo "Open your browser and navigate to:"
echo "  http://localhost:$PORT/"
echo ""
echo "Press Ctrl+C to stop the server"
echo "============================================"
echo ""

# Change to webtool directory
cd "$SCRIPT_DIR" || exit 1

# Start HTTP server with WEBrick (gem install webrick if not installed)
ruby -e "
require 'webrick'
server = WEBrick::HTTPServer.new(Port: $PORT, DocumentRoot: '.')
trap('INT') { server.shutdown }
server.start
"
