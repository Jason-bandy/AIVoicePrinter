from __future__ import print_function
import os
import subprocess

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
    EXEC_PATH   = r'/opt/gcc-arm-none-eabi-5_4-2016q3/bin'
else:
    print('Please make sure your toolchains is GNU GCC!')
    exit(0)

if os.getenv('RTT_EXEC_PATH'):
    EXEC_PATH = os.getenv('RTT_EXEC_PATH')
    rtt_toolchain = subprocess.check_output(EXEC_PATH + "/arm-none-eabi-gcc -dumpversion", shell=True).strip()

if isinstance(rtt_toolchain, bytes):
    rtt_toolchain = rtt_toolchain.decode()

if rtt_toolchain != SDK_TOOLCHAIN:
    print('Please make sure your toolchains version is %s!' % SDK_TOOLCHAIN)
    exit(0)

print('EXEC_PATH is: %s' % EXEC_PATH)

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
