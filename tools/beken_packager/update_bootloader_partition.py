#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Update partition table in bootloader binary for macOS builds.
This replaces the Linux-only rt_partition_tool_cli functionality.

Usage: python3 update_bootloader_partition.py <bootloader.bin> <partition.json> <output.bin>
"""

import json
import struct
import sys
import os
import shutil

# Partition table entry structure (64 bytes each):
# [0:4]   magic: "01PE" (0x45503130)
# [4:16]  name: partition name (12 bytes string)
# [16:28] empty: padding (12 bytes)
# [28:44] flash_name: flash device name (16 bytes string)
# [44:48] empty: padding (4 bytes)
# [48:52] empty: padding (4 bytes)
# [52:56] offset: partition start address (4 bytes uint32 LE)
# [56:60] length: partition length (4 bytes uint32 LE)
# [60:64] extra: unknown data (4 bytes)

ENTRY_SIZE = 64
OFFSET_FIELD = 52
LENGTH_FIELD = 56
FLASH_NAME_FIELD = 28
FLASH_NAME_SIZE = 16

# Known entry offsets in bootloader (from analysis)
ENTRY_OFFSETS = {
    'bootloader': 0xd2fc,
    'app': 0xd33c,
    'download': 0xd37c,
}

MAGIC = b'01PE'


def find_entry_offset(data, entry_name):
    """Find the offset of a partition entry by name."""
    # Try known offsets first
    if entry_name in ENTRY_OFFSETS:
        known_offset = ENTRY_OFFSETS[entry_name]
        # Verify magic at known offset
        if data[known_offset:known_offset+4] == MAGIC:
            # Verify name
            name = data[known_offset+4:known_offset+16].rstrip(b'\x00').decode('ascii', errors='ignore')
            if name.lower() == entry_name.lower():
                return known_offset

    # Search for entry by name
    search_pattern = MAGIC + entry_name.encode('ascii').ljust(12, b'\x00')
    pos = 0
    while pos < len(data) - len(search_pattern):
        pos = data.find(search_pattern, pos)
        if pos != -1:
            return pos
        pos += 1

    return None


def update_entry(data, entry_offset, offset_val, length_val, flash_name=None):
    """Update offset and length fields in a partition entry."""
    data = bytearray(data)

    # Write offset (little endian)
    struct.pack_into('<I', data, entry_offset + OFFSET_FIELD, offset_val)

    # Write length (little endian)
    struct.pack_into('<I', data, entry_offset + LENGTH_FIELD, length_val)

    # Update flash name if provided
    if flash_name:
        flash_bytes = flash_name.encode('ascii').ljust(FLASH_NAME_SIZE, b'\x00')
        data[entry_offset + FLASH_NAME_FIELD:entry_offset + FLASH_NAME_FIELD + FLASH_NAME_SIZE] = flash_bytes

    return bytes(data)


def update_bootloader_partition(bootloader_path, partition_json_path, output_path):
    """Update partition table in bootloader binary."""

    # Read bootloader binary
    with open(bootloader_path, 'rb') as f:
        bootloader_data = f.read()

    # Read partition JSON
    with open(partition_json_path, 'r', encoding='utf-8') as f:
        partition_config = json.load(f)

    print(f"Loaded bootloader: {len(bootloader_data)} bytes")
    print(f"Loaded partition config: {len(partition_config['part_table'])} partitions")

    # Update each partition
    modified_data = bootloader_data
    for entry in partition_config['part_table']:
        name = entry['name']

        # Parse offset (handle hex strings)
        offset_str = entry['offset']
        if isinstance(offset_str, str) and offset_str.startswith('0x'):
            offset_val = int(offset_str, 16)
        else:
            offset_val = int(offset_str)

        # Parse length
        length_val = int(entry['len'])

        # Get flash name
        flash_name = entry.get('flash_name', '')

        # Find entry in bootloader
        entry_offset = find_entry_offset(modified_data, name)

        if entry_offset is None:
            print(f"Warning: Partition '{name}' not found in bootloader, skipping")
            continue

        # Update entry
        modified_data = update_entry(
            modified_data,
            entry_offset,
            offset_val,
            length_val,
            flash_name if flash_name else None
        )

        print(f"Updated '{name}': offset=0x{offset_val:08x}, length={length_val} (0x{length_val:x})")

    # Write output
    output_dir = os.path.dirname(output_path)
    if output_dir and not os.path.exists(output_dir):
        os.makedirs(output_dir)

    with open(output_path, 'wb') as f:
        f.write(modified_data)

    print(f"Written updated bootloader: {output_path} ({len(modified_data)} bytes)")

    # Verify
    print("\nVerification - Reading back updated entries:")
    for entry in partition_config['part_table']:
        name = entry['name']
        entry_offset = find_entry_offset(modified_data, name)
        if entry_offset:
            off = struct.unpack('<I', modified_data[entry_offset + OFFSET_FIELD:entry_offset + OFFSET_FIELD + 4])[0]
            length = struct.unpack('<I', modified_data[entry_offset + LENGTH_FIELD:entry_offset + LENGTH_FIELD + 4])[0]
            print(f"  {name}: offset=0x{off:08x} ({off}), length={length} (0x{length:x})")


if __name__ == '__main__':
    if len(sys.argv) != 4:
        print("Usage: python3 update_bootloader_partition.py <bootloader.bin> <partition.json> <output.bin>")
        sys.exit(1)

    bootloader_path = sys.argv[1]
    partition_json_path = sys.argv[2]
    output_path = sys.argv[3]

    if not os.path.exists(bootloader_path):
        print(f"Error: Bootloader file not found: {bootloader_path}")
        sys.exit(1)

    if not os.path.exists(partition_json_path):
        print(f"Error: Partition JSON not found: {partition_json_path}")
        sys.exit(1)

    update_bootloader_partition(bootloader_path, partition_json_path, output_path)
