# -*- coding: utf-8 -*-

import os
import sys
import json
import shutil

out_json = {
    "magic": "RT-Thread",
    "version": "0.1",
    "count": 4,
    "section": [
        {
            "firmware": "bk7231n_bootloader_enc.bin",
            "version": "1.00",
            "partition": "bootloader",
            "start_addr": "0x00000000",
            "size": "68K"
        },
        {
            "firmware": "bk7231_common_1.0.1_enc.bin",
            "version": "1.00",
            "partition": "app",
            "start_addr": "0x00011000",
            "size": "1150832"
        }
    ]
}

def generate_beken_packager_json(bootloader_str, firmware_str, out_path):
    global out_json 
    out_json["section"][0]["firmware"] = bootloader_str
    out_json["section"][1]["firmware"] = firmware_str
    out_json = json.dumps(out_json, sort_keys=True, indent=4)

    print(out_json)

    with open(str(out_path), "w") as f:
        f.write(out_json)

if __name__=='__main__':
    # from --beken=xxx
    beken_str = sys.argv[1]
    bootloader_str = ""
    src_path = ""
    firmware_str = "../../rtthread.bin"
    out_path = "tool/beken_packager/config.json"
    if beken_str == "bk7231u":
        bootloader_str = "bootloader_7231u_uart2_v1.0.6.bin"
    elif beken_str == "bk7231n":
        bootloader_str = "bootloader_7231N_uart2_v1.0.6.bin"
    elif beken_str == "bk7271":
        bootloader_str = "bootloader_7271_uart2_v1.0.6.bin"
    else:
        bootloader_str = "bootloader_7251_uart2_v1.0.6.bin"
        src_path = "tool/beken_packager/config_bk7251_2M.json"

    # copy or generate json file
    if src_path == "":
        generate_beken_packager_json(bootloader_str, firmware_str, out_path)
    else:
        print('copy ' + src_path + ' to ' + out_path)
        shutil.copy(src_path, out_path)

    # run beken packager
    if sys.platform == 'win32':
        os.system("cd tool/beken_packager && beken_packager")
    else:
        os.system("cd tool/beken_packager && ./beken_packager")