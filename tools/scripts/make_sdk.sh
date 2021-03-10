#! /bin/bash
# generate SDK after a new git clone and update the SDK/OSK revisions

# generate libs
./tools/scripts/generate_beken_libs.sh bk7231n
if [ $? != 0 ]; then
	echo "make bk7231n libs fail"
	exit 1
fi

./tools/scripts/generate_beken_libs.sh bk7231u
if [ $? != 0 ]; then
	echo "make bk7231u libs fail"
	exit 1
fi

./tools/scripts/generate_beken_libs.sh bk7251
if [ $? != 0 ]; then
	echo "make bk7251 libs fail"
	exit 1
fi

#./tools/scripts/generate_beken_libs.sh bk7271
#if [ $? != 0 ]; then
#	echo "make bk7271 libs fail"
#	exit 1
#fi

# make clean and remove output files
echo "clean unused files ..."
rm -rf config/sys_config.h
rm -f rtconfig.h
rm -rf build
rm -rf bugzilla
rm -f README.md
rm -f .platform
rm -rf ./beken378/bugzilla
rm -rf ./beken378/ip_ax
rm -f ./beken378/README.md
rm -rf ./samples/story_xyos
rm -rf ./samples/wantong
rm -rf .sconsign.dblite
find ./ -name "*.pyc" | xargs rm -rf

# clean lib source files
./tools/scripts/clean_src_files.sh
if [ $? != 0 ]; then
	echo "clean src files fail"
	exit 1
fi

if [ "${RTT_EXEC_PATH}" = "" ]; then
	echo "env RTT_EXEC_PATH is not set, skip packaging toolchain"
	exit 0
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
