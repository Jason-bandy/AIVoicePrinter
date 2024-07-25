# -*- coding: utf-8 -*-

import os
import sys
import json
import shutil

out_json_1M = {
    "magic": "RT-Thread",
    "version": "0.1",
    "count": 2,
    "section": [
        {
            "firmware": "bootloader_bk7231n_uart2_v1.0.13.bin",
            "version": "1M.1220",
            "partition": "bootloader",
            "start_addr": "0x00000000",
            "size": "65280"
        },
        {
            "firmware": "../../out/beken7231_bsp.bin",
            "version": "1M.1220",
            "partition": "app",
            "start_addr": "0x00011000",
            "size": "816K"
        }
    ]
}

out_json_2M = {
    "magic": "RT-Thread",
    "version": "0.1",
    "count": 2,
    "section": [
        {
            "firmware": "bootloader_bk7231n_uart2_v1.0.13.bin",
            "version": "2M.1220",
            "partition": "bootloader",
            "start_addr": "0x00000000",
            "size": "65280"
        },
        {
            "firmware": "../../out/beken7231_bsp.bin",
            "version": "2M.1220",
            "partition": "app",
            "start_addr": "0x00011000",
            "size": "1156K"
        }
    ]
}

out_json_4M = {
    "magic": "RT-Thread",
    "version": "0.1",
    "count": 2,
    "section": [
        {
            "firmware": "bootloader_bk7231n_uart2_v1.0.13.bin",
            "version": "4M.1220",
            "partition": "bootloader",
            "start_addr": "0x00000000",
            "size": "65280"
        },
        {
            "firmware": "../../out/beken7231_bsp.bin",
            "version": "4M.1220",
            "partition": "app",
            "start_addr": "0x00011000",
            "size": "2890K"
        }
    ]
}

out_json_8M = {
    "magic": "RT-Thread",
    "version": "0.1",
    "count": 2,
    "section": [
        {
            "firmware": "bootloader_bk7231n_uart2_v1.0.13.bin",
            "version": "8M.1220",
            "partition": "bootloader",
            "start_addr": "0x00000000",
            "size": "65280"
        },
        {
            "firmware": "../../out/beken7231_bsp.bin",
            "version": "8M.1220",
            "partition": "app",
            "start_addr": "0x00011000",
            "size": "2076K"
        }
    ]
}

def gather_out_files(bootloader_str, full_image, uart_image):
    if sys.platform == 'win32':
        cmd_cp = "copy "
        cmd_mv = "move "
        cmd_rm = "rd/s/q "
    else:
        cmd_cp = "cp "
        cmd_mv = "mv "
        cmd_rm = "rm -rf "
    out_folder = " out"
    os.system(cmd_rm + out_folder)
    os.system("mkdir" + out_folder)
    os.system(cmd_mv + "rtthread.bin" + out_folder)
    os.system(cmd_mv + "rtthread.elf" + out_folder)
    os.system(cmd_mv + "rtthread.map" + out_folder)
    os.system(cmd_mv + full_image + out_folder)
    os.system(cmd_mv + uart_image + out_folder)
    os.system(cmd_cp + bootloader_str + out_folder)
    print('rtthread.bin and other generated files were moved to folder %s' % out_folder)

if __name__=='__main__':
    # from --beken=xxx
    beken_str = sys.argv[1]

    # get flash size
    flash_size = sys.argv[2]
    if flash_size.startswith("0x"):
        flash_size = int(flash_size.replace("0x", ""),16)
    else:
        flash_size = int(flash_size)

    #select json with flash size
    if flash_size >= 0x800000:
        out_json = out_json_8M
    elif flash_size >= 0x400000:
        out_json = out_json_4M
    elif flash_size >= 0x200000:
        out_json = out_json_2M
    elif flash_size >= 0x100000:
        out_json = out_json_1M
    else:
        out_json = out_json_2M

    # generate out bin name
    full_image = "all_" + out_json["section"][0]["version"] + ".bin"
    uart_image = "rtthread_uart_" + out_json["section"][0]["version"] + ".bin"

    # generate config.json
    bootloader_str = ""
    firmware_str = "rtthread.bin"
    out_json_str = "config.json"
    if beken_str == "bk7231u":
        bootloader_str = "tools/beken_packager/bootloader_bk7231u_uart2_v1.0.13.bin"
    elif beken_str == "bk7231n":
        bootloader_str = "tools/beken_packager/bootloader_bk7231n_uart1_v1.0.13.bin"
    elif beken_str == "bk7236":
        bootloader_str = "tools/beken_packager/bootloader_bk7236_uart2_v1.0.8.bin"
    elif beken_str == "bk7238":
        bootloader_str = "tools/beken_packager/bootloader_bk7238_uart1_v1.0.14.bin"
    elif beken_str == "bk7252n":
        bootloader_str = "tools/beken_packager/bootloader_bk7252n_uart1_v1.0.14.bin"
    elif beken_str == "bk7271":
        bootloader_str = "tools/beken_packager/bootloader_bk7271_uart2_v1.0.8.bin"
    else:
        bootloader_str = "tools/beken_packager/bootloader_bk7251_uart2_v1.0.13.bin"

    out_json["section"][0]["firmware"] = bootloader_str
    out_json["section"][1]["firmware"] = firmware_str
    out_json = json.dumps(out_json, sort_keys=True, indent=4)
    print(out_json)
    with open(str(out_json_str), "w") as f:
            f.write(out_json)

    # run beken packager
    if sys.platform == 'win32':
        os.system("tools\\beken_packager\\beken_packager")
    else:
        os.system("./tools/beken_packager/beken_packager")

    # gather out files
    gather_out_files(bootloader_str, full_image, uart_image)
