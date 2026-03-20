#!/bin/bash
# Setup script for macOS build environment
# This script helps configure the ARM GCC toolchain path

echo "======================================"
echo "Beken RT-Thread SDK - macOS Setup"
echo "======================================"
echo ""

# Check if arm-none-eabi-gcc is installed
if command -v arm-none-eabi-gcc &> /dev/null; then
    echo "✓ Found arm-none-eabi-gcc in PATH"
    GCC_PATH=$(dirname $(which arm-none-eabi-gcc))
    echo "  Location: $GCC_PATH"
    echo ""
    echo "Setting RTT_EXEC_PATH=$GCC_PATH"
    export RTT_EXEC_PATH=$GCC_PATH
else
    echo "⚠ arm-none-eabi-gcc not found in PATH"
    echo ""
    
    # Check common installation locations
    if [ -f "/opt/homebrew/bin/arm-none-eabi-gcc" ]; then
        echo "✓ Found ARM GCC at /opt/homebrew/bin (Apple Silicon Mac)"
        export RTT_EXEC_PATH="/opt/homebrew/bin"
    elif [ -f "/usr/local/bin/arm-none-eabi-gcc" ]; then
        echo "✓ Found ARM GCC at /usr/local/bin (Intel Mac)"
        export RTT_EXEC_PATH="/usr/local/bin"
    else
        echo "✗ ARM GCC toolchain not found"
        echo ""
        echo "Please install it using Homebrew:"
        echo "  brew install arm-none-eabi-gcc"
        echo ""
        echo "Or download from: https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads"
        exit 1
    fi
fi

# Verify the setup
echo ""
echo "Testing toolchain..."
$RTT_EXEC_PATH/arm-none-eabi-gcc --version | head -n 1
echo ""
echo "======================================"
echo "Environment configured successfully!"
echo "======================================"
echo ""
echo "You can now run:"
echo "  scons          - to build the project"
echo "  scons --target=mdk5 - to generate Keil MDK v5 project"
echo ""
echo "Or use VSCode tasks (Ctrl+Shift+B)"
echo ""
