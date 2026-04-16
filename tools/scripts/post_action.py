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
    # macOS uses shutil for file operations
    if sys.platform == 'darwin':
        import shutil
        out_folder = "out"
        if os.path.exists(out_folder):
            shutil.rmtree(out_folder)
        os.makedirs(out_folder)
        shutil.move("rtthread.bin", os.path.join(out_folder, "rtthread.bin"))
        shutil.move("rtthread.elf", os.path.join(out_folder, "rtthread.elf"))
        shutil.move("rtthread.map", os.path.join(out_folder, "rtthread.map"))
        shutil.move(full_image, os.path.join(out_folder, full_image))
        shutil.move(uart_image, os.path.join(out_folder, uart_image))
        shutil.move(firmware_rbl, os.path.join(out_folder, firmware_rbl))
        shutil.copy(bootloader_str, os.path.join(out_folder, os.path.basename(bootloader_str)))
        print('rtthread.bin and other generated files were moved to folder %s' % out_folder)
    else:
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
    # macOS doesn't have the Linux binary tools, use Python-native alternative
    if sys.platform == 'darwin':
        print("macOS build - using Python-native post-processing")
        # Just verify firmware exists
        if not os.path.exists("rtthread.bin"):
            print("Error: rtthread.bin not found")
            sys.exit(1)
        firmware_size = os.path.getsize("rtthread.bin")
        print("Firmware built successfully: {} bytes ({:.2f} MB)".format(firmware_size, firmware_size/1024/1024))

        # Generate partition.json and packager.json from BkFlashPartition.h info
        # For bk7252n 2M config: boot=0x11000, app=0x13D000, ota=0x92000
        partition_data = {
            "magic": "RT-Thread",
            "version": "1.0",
            "part_table": [
                {"name": "boot", "len": 69632, "offset": 0},
                {"name": "app", "len": 1261568, "offset": 69632},
                {"name": "download", "len": 602112, "offset": 1331200}
            ]
        }
        packager_data = {
            "section": [
                {"name": "bootloader", "size": "69632"},
                {"name": "app", "size": "1261568"}
            ]
        }
        with open("partition.json", 'w') as f:
            json.dump(partition_data, f, indent=4)
        with open("packager.json", 'w') as f:
            json.dump(packager_data, f, indent=4)
        print("Generated partition.json and packager.json for macOS")
        # Continue to generate merged firmware files
    else:
        if os.name == 'nt':
            cmd_str = '"' + sys.executable + '" tools\\beken_packager\\gen_partition tools\\beken_packager\\flash_partition.o'
        else:
            cmd_str = '"' + sys.executable + '" tools/beken_packager/gen_partition tools/beken_packager/flash_partition.o'
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

    # replace partition info in boot
    if sys.platform == 'darwin':
        # Use Python-based partition table updater on macOS
        print("Updating bootloader partition table (macOS)...")
        import shutil
        # Ensure out directory exists
        if not os.path.exists("out"):
            os.makedirs("out")
        updated_bootloader = "out/bootloader_updated.bin"
        # Call the update function directly instead of subprocess
        try:
            from tools.beken_packager.update_bootloader_partition import update_bootloader_partition
            update_bootloader_partition(bootloader_str, boot_json.strip(), updated_bootloader)
            # Replace original bootloader with updated version
            shutil.copy(updated_bootloader, bootloader_str)
            print("Bootloader partition table updated successfully")
        except Exception as e:
            print(f"Warning: Failed to update bootloader partition table: {e}")
    else:
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

    # Skip OTA packaging command building on macOS
    if sys.platform != 'darwin':
        if os.name == 'nt':
            ota_pack_cmd = ".\\tools\\rtt_ota\\rt_ota_packaging_tool_cli.exe -f " + firmware_str
        else:
            ota_pack_cmd = "./tools/rtt_ota/rt_ota_packaging_tool_cli-x86 -f " + firmware_str
        ota_pack_cmd = ota_pack_cmd + " -v " + out_json["section"][0]["version"]
        ota_pack_cmd = ota_pack_cmd + " -o " + firmware_rbl
        ota_pack_cmd = ota_pack_cmd + " -p " + out_json["section"][1]["partition"]
        ota_pack_cmd = ota_pack_cmd + " -c lzma -s aes -k 0123456789ABCDEF0123456789ABCDEF -i 0123456789ABCDEF"

    # Save config.json (but keep out_json as dict for later use)
    out_json_str_content = json.dumps(out_json, sort_keys=True, indent=4)
    print(out_json_str_content)
    with open(str(out_json_str), "w") as f:
            f.write(out_json_str_content)

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

    # run beken packager (skip on macOS - Linux binary)
    if sys.platform == 'win32':
        os.system("tools\\beken_packager\\beken_packager")
    elif sys.platform == 'darwin':
        print("Skipping beken_packager on macOS (Linux binary)")
        # Generate merged firmware files manually on macOS
        print("Generating merged firmware files for macOS...")
        with open(bootloader_str, 'rb') as f:
            bootloader_data = f.read()
        with open(firmware_str, 'rb') as f:
            firmware_data = f.read()

        # full image: bootloader + padding + firmware
        boot_size = int(out_json["section"][0]["size"])
        app_offset = int(out_json["section"][1]["start_addr"], 16)
        padding_size = app_offset - len(bootloader_data)

        # full image: bootloader + padding + firmware
        full_image_data = bootloader_data + b'\xFF' * padding_size + firmware_data
        with open(full_image, 'wb') as f:
            f.write(full_image_data)

        # uart image: same as full image for now
        with open(uart_image, 'wb') as f:
            f.write(full_image_data)

        print("Generated: {} ({} bytes)".format(full_image, len(full_image_data)))
        print("Generated: {} ({} bytes)".format(uart_image, len(full_image_data)))
    else:
        os.system("./tools/beken_packager/beken_packager")

    # excute cmd for ota packager (skip on macOS - Linux binary)
    if sys.platform == 'darwin':
        print("Skipping OTA packaging on macOS (Linux binary)")
        # Create empty rbl file as placeholder for build to succeed
        with open(firmware_rbl, 'w') as f:
            f.write('')
    else:
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
                print("\033[33m WARNING: rbl size:{} larger than ota size:{}, OTA update not available\033[0m".format(rbl_size, ota_size))

    # gather out files
    gather_out_files(bootloader_str, full_image, uart_image, firmware_rbl)

    os.remove("partition.json")
    os.remove("packager.json")
