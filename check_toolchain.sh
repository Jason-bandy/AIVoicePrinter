#!/bin/bash
# Diagnostic script to check ARM GCC toolchain installation on macOS

echo "======================================"
echo "ARM GCC Toolchain Diagnostic"
echo "======================================"
echo ""

# Check if arm-none-eabi-gcc is available
if command -v arm-none-eabi-gcc &> /dev/null; then
    echo "✓ arm-none-eabi-gcc found in PATH"
    GCC_PATH=$(which arm-none-eabi-gcc)
    echo "  Path: $GCC_PATH"
    echo ""
    
    # Get version
    echo "Version information:"
    arm-none-eabi-gcc --version | head -n 2
    echo ""
    
    # Get sysroot
    echo "Sysroot path:"
    arm-none-eabi-gcc --print-sysroot
    echo ""
    
    # Check for sys/stat.h
    SYSROOT=$(arm-none-eabi-gcc --print-sysroot)
    if [ -f "$SYSROOT/usr/include/sys/stat.h" ]; then
        echo "✓ Found sys/stat.h in sysroot"
        echo "  Location: $SYSROOT/usr/include/sys/stat.h"
    else
        echo "✗ sys/stat.h NOT found in sysroot"
        echo "  Expected at: $SYSROOT/usr/include/sys/stat.h"
        echo ""
        echo "This might indicate an incomplete toolchain installation."
        echo "Try reinstalling:"
        echo "  brew reinstall arm-none-eabi-gcc"
    fi
    echo ""
    
    # List include directories
    echo "Include search paths:"
    arm-none-eabi-gcc -xc -E -v - < /dev/null 2>&1 | grep -A 20 "#include <...>"
    echo ""
    
else
    echo "✗ arm-none-eabi-gcc NOT found in PATH"
    echo ""
    echo "Please install ARM GCC toolchain:"
    echo "  brew install arm-none-eabi-gcc"
    exit 1
fi

echo "======================================"
echo "Diagnostic complete"
echo "======================================"
