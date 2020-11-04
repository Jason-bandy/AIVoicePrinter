#! /bin/bash
# generate SDK after a new git clone and update the SDK/OSK revisions

# generate libs

# make clean and remove output files
echo "clean unused files ..."
rm -rf config/sys_config.h
rm -rf build
rm -rf bugzilla
rm -f README.md
rm -rf ./beken378/bugzilla
rm -f ./beken378/README.md

# clean lib source files
./tool/scripts/clean_lib_files.sh

if [ "${RTT_EXEC_PATH}" = "" ]; then
	echo "env RTT_EXEC_PATH is not set"
	exit 1
fi

TOOLCHAIN_DIR=$(dirname "$RTT_EXEC_PATH")
TOOLCHAIN_BASE=$(basename "$TOOLCHAIN_DIR")

echo "TOOLCHAIN_DIR: ${TOOLCHAIN_DIR}"

# add toolchain directory
if [ ! -d toolchain ]; then
	echo "packaging toolchain ..."
	mkdir toolchain
	cd toolchain
	cp -rf ${TOOLCHAIN_DIR} ${TOOLCHAIN_BASE}
	tar -cf ${TOOLCHAIN_BASE}.tar.bz2 ${TOOLCHAIN_BASE}
	rm -rf ${TOOLCHAIN_BASE}
	cd ..
fi

echo "make SDK done."
