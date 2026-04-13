from __future__ import print_function
import os
import subprocess
import platform

# toolchains options
ARCH        ='arm'
CPU         ='beken'
CROSS_TOOL  ='gcc'
SDK_TOOLCHAIN = '5.4.1'

if os.getenv('RTT_ROOT'):
    RTT_ROOT = os.getenv('RTT_ROOT')
else:
    RTT_ROOT = os.path.join(os.path.normpath(os.getcwd()), 'rt-thread')

print('RTT_ROOT is: %s' % RTT_ROOT)

if os.getenv('RTT_CC'):
    CROSS_TOOL = os.getenv('RTT_CC')

print('CROSS_TOOL is: %s' % CROSS_TOOL)
if  CROSS_TOOL == 'gcc':
    PLATFORM    = 'gcc'

    # Auto-detect OS and set appropriate path
    system_platform = platform.system()

    # Priority 1: Use project-local toolchain (for consistent builds across team)
    local_toolchain = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'toolchain', 'gcc-arm-none-eabi-5_4-2016q3', 'bin')
    if os.path.exists(local_toolchain):
        EXEC_PATH = local_toolchain
        print('Using project-local toolchain: %s' % EXEC_PATH)
    elif system_platform == 'Darwin':  # macOS
        # Try common macOS installation paths
        possible_paths = [
            '/opt/homebrew/bin',  # Apple Silicon Mac
            '/usr/local/bin',     # Intel Mac
            '/opt/gcc-arm-none-eabi-5_4-2016q3/bin',
        ]

        # Check which path exists
        EXEC_PATH = None
        for path in possible_paths:
            if os.path.exists(path):
                EXEC_PATH = path
                break

        if EXEC_PATH is None:
            # Default to Homebrew path
            EXEC_PATH = '/opt/homebrew/bin' if os.path.exists('/opt/homebrew/bin') else '/usr/local/bin'

    elif system_platform == 'Linux':
        EXEC_PATH = '/usr/bin'
    else:  # Windows
        possible_paths_win = [
            r'C:\Program Files (x86)\GNU Tools ARM Embedded\5.4 2016q3\bin',
            r'C:\Program Files\GNU Tools ARM Embedded\5.4 2016q3\bin',
            r'C:\gcc-arm-none-eabi-5_4-2016q3\bin',
        ]
        EXEC_PATH = possible_paths_win[0]  # default
        for p in possible_paths_win:
            if os.path.exists(p):
                EXEC_PATH = p
                break
else:
    print('Please make sure your toolchains is GNU GCC!')
    exit(0)

if os.getenv('RTT_EXEC_PATH'):
    EXEC_PATH = os.getenv('RTT_EXEC_PATH')

# Check toolchain version (skip on macOS if version check causes issues)
try:
    gcc_cmd = '"%s"' % os.path.join(EXEC_PATH, "arm-none-eabi-gcc") + " -dumpversion"
    rtt_toolchain = subprocess.check_output(gcc_cmd, shell=True).strip()
    
    if isinstance(rtt_toolchain, bytes):
        rtt_toolchain = rtt_toolchain.decode()
    
    # Only enforce version check on Windows, allow flexibility on macOS/Linux
    if system_platform == 'Windows' and rtt_toolchain != SDK_TOOLCHAIN:
        print('Please make sure your toolchains version is %s!' % SDK_TOOLCHAIN)
        exit(0)
    else:
        print('Using GCC toolchain version: %s (located at: %s)' % (rtt_toolchain.strip(), EXEC_PATH))
except Exception as e:
    print('Warning: Could not verify toolchain version: %s' % str(e))
    print('Continuing with build anyway...')

BUILD = 'release'
# BUILD = 'debug'

if PLATFORM == 'gcc':
    # toolchains
    PREFIX  = 'arm-none-eabi-'
    CC      = PREFIX + 'gcc'
    CXX     = PREFIX + 'g++'
    AS      = PREFIX + 'gcc'
    AR      = PREFIX + 'ar'
    LINK    = PREFIX + 'g++'
    TARGET_EXT = 'elf'
    SIZE    = PREFIX + 'size'
    OBJDUMP = PREFIX + 'objdump'
    OBJCPY  = PREFIX + 'objcopy'

    DEVICE  = ' -mcpu=arm968e-s -mthumb-interwork -mthumb -ffunction-sections -fdata-sections'
    CFLAGS  = DEVICE + ' -Iconfig' + ' -Irelease' + ' -Werror' + ' -Wall' + ' -Wno-format' + ' -Wno-unknown-pragmas'
    _lwip_root = os.path.join(RTT_ROOT, 'components', 'net', 'lwip-2.0.2', 'src').replace('\\', '/')
    _lwip_inc = _lwip_root + '/include'
    CFLAGS += ' -I' + _lwip_root
    CFLAGS += ' -I' + _lwip_inc
    CFLAGS += ' -I' + _lwip_inc + '/ipv4'
    AFLAGS  = ' -c' + DEVICE + ' -x assembler-with-cpp -Iconfig'
    LFLAGS  = DEVICE + ' -nostartfiles -Wl,--gc-sections,-Map=rtthread.map,-cref,-u,system_vectors -T link.lds'
    CPATH   = ''
    LPATH   = ''

    if BUILD == 'debug':
        CFLAGS += ' -O0 -gdwarf-2'
        AFLAGS += ' -gdwarf-2'
    else:
        CFLAGS += ' -Os -gdwarf-2'
        AFLAGS += ' -gdwarf-2'

    CXXFLAGS = CFLAGS

DUMP_ACTION = OBJDUMP + ' -D -S $TARGET > rtt.asm\n'
POST_ACTION = OBJCPY + ' -O binary $TARGET rtthread.bin\n' + SIZE + ' $TARGET \n'
