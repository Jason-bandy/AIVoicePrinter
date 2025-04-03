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

# 65280 not error with bootloader.bin <= 60K, but 68K will be better
# boot_size = 65280 Bytes = 60K / 32 * 34
#
# 2890K not error with app.bin <= 2720K, but if you want to OTA,
# you should check the OTA partition param:
# OTA_offset >= (0x11000 + 2890K) = 0x2E3800
# OTA_length <= (next_partition_offset - OTA_offset)
# suggest change 2890K => 2516K,
# because of the efficiency of encode method is almost 0.6,
# and APP + OTA will fill with the flash space from 0x11000 to 0x3e0000
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
            "size": "2516K"
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

def gather_out_files(bootloader_str, full_image, uart_image, firmware_rbl):
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
    os.system(cmd_mv + firmware_rbl + out_folder)
    os.system(cmd_cp + bootloader_str + out_folder)
    print('rtthread.bin and other generated files were moved to folder %s' % out_folder)

if __name__=='__main__':
    if os.name == 'nt':
        cmd_str = "python3 tools\\beken_packager\\gen_partition tools\\beken_packager\\flash_partition.o"
    else:
        cmd_str = "python3 tools/beken_packager/gen_partition tools/beken_packager/flash_partition.o"
    print(cmd_str)
    os.system(cmd_str)
    os.remove("tools/beken_packager/flash_partition.o")

    # from --beken=xxx
    beken_str = sys.argv[1]

    # get flash size
    flash_size = sys.argv[2]
    if flash_size.startswith("0x"):
        flash_size = int(flash_size.replace("0x", ""),16)
    else:
        flash_size = int(flash_size)

    boot_json = " partition.json"
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
        bootloader_str = "tools/beken_packager/bootloader_bk7231u_uart2_v1.0.15.bin"
    elif beken_str == "bk7231n":
        bootloader_str = "tools/beken_packager/bootloader_bk7231n_uart1_v1.0.15.bin"
    elif beken_str == "bk7236":
        bootloader_str = "tools/beken_packager/bootloader_bk7236_uart2_v1.0.8.bin"
    elif beken_str == "bk7238":
        bootloader_str = "tools/beken_packager/bootloader_bk7238_uart1_v1.0.15.bin"
    elif beken_str == "bk7252n":
        bootloader_str = "tools/beken_packager/bootloader_bk7252n_uart1_v1.0.15.bin"
    elif beken_str == "bk7271":
        bootloader_str = "tools/beken_packager/bootloader_bk7271_uart2_v1.0.8.bin"
    else:
        bootloader_str = "tools/beken_packager/bootloader_bk7251_uart2_v1.0.15.bin"

    # replace partition info in booot
    if os.name == 'nt':
        cmd_str = "tools\\rt_partition_tool\\rt_partition_tool_cli.exe " + bootloader_str +  boot_json
    else:
        cmd_str = "tools/rt_partition_tool/rt_partition_tool_cli " + bootloader_str + boot_json
    print(cmd_str)
    os.system(cmd_str)

    # set right size for boot and app
    with open("packager.json", 'r') as file:
        packager_json = json.load(file)
        if((packager_json["section"][0]["name"]) == "bootloader") :
            out_json["section"][0]["size"] = packager_json["section"][0]["size"]
        if((packager_json["section"][1]["name"]) == "app") :
            out_json["section"][1]["size"] = packager_json["section"][1]["size"]

    out_json["section"][0]["firmware"] = bootloader_str
    out_json["section"][1]["firmware"] = firmware_str

    # cmd for ota packager rbl
    if("bin" in firmware_str) :
        firmware_rbl = firmware_str.replace(".bin", ".rbl")
    else:
        firmware_rbl = firmware_str + ".rbl"
    if os.name == 'nt':
        ota_pack_cmd = ".\\tools\\rtt_ota\\rt_ota_packaging_tool_cli.exe -f " + firmware_str
    else:
        ota_pack_cmd = "./tools/rtt_ota/rt_ota_packaging_tool_cli-x86 -f " + firmware_str
    ota_pack_cmd = ota_pack_cmd + " -v " + out_json["section"][0]["version"]
    ota_pack_cmd = ota_pack_cmd + " -o " + firmware_rbl
    ota_pack_cmd = ota_pack_cmd + " -p " + out_json["section"][1]["partition"]
    ota_pack_cmd = ota_pack_cmd + " -c lzma -s aes -k 0123456789ABCDEF0123456789ABCDEF -i 0123456789ABCDEF"

    out_json = json.dumps(out_json, sort_keys=True, indent=4)
    print(out_json)
    with open(str(out_json_str), "w") as f:
            f.write(out_json)

    # check firmware size, should less than app size
    firmware_size = os.path.getsize(firmware_str)
    app_size = 0
    with open("partition.json", 'r') as file:
        partition_json = json.load(file)
        if( partition_json["part_table"][1]["name"] == "app") :
            app_size = int(partition_json["part_table"][1]["len"])
            #print(firmware_size, app_size)
            if(firmware_size > app_size) :
                raise ValueError("\033[31m firmware size:{} larger than app size:{}\033[0m".format(firmware_size, app_size))

    # run beken packager
    if sys.platform == 'win32':
        os.system("tools\\beken_packager\\beken_packager")
    else:
        os.system("./tools/beken_packager/beken_packager")

    # excute cmd for ota packager
    print(ota_pack_cmd)
    os.system(ota_pack_cmd)

    # check rbl size, should less than ota size
    rbl_size = os.path.getsize(firmware_rbl)
    ota_size = 0
    with open("partition.json", 'r') as file:
        partition_json = json.load(file)
        if( partition_json["part_table"][2]["name"] == "download") :
            ota_size = int(partition_json["part_table"][2]["len"])
            #print(rbl_size, ota_size)
            if(rbl_size > ota_size) :
                raise ValueError("\033[31m rbl size:{} larger than ota size:{}\033[0m".format(rbl_size, ota_size))

    # gather out files
    gather_out_files(bootloader_str, full_image, uart_image, firmware_rbl)

    os.remove("partition.json")
    os.remove("packager.json")
