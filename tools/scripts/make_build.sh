#! /bin/bash
# Usage: make_build.sh [platform]

source ./tools/scripts/build_include.sh

if [ "$1" != "" ]; then
	PLATFORM=$1
else
	PLATFORM=bk7251
fi

validate_platform $PLATFORM
if [ $? != 0 ]; then
	exit 1
fi

echo "making build for $PLATFORM ..."

SDK_DIR=$(pwd)/beken378

if [ -f ${SDK_DIR}/ip/lmac/src/rx/rxl/rxl_cntrl.c ]; then
	scons --beken=$PLATFORM --buildlibs
	if [ $? != 0 ]; then
		echo "make build eror!"
		exit 1
	fi
fi

scons --beken=$PLATFORM -j8
if [ $? != 0 ]; then
	echo "make build error!"
	exit 1
else
	echo "make build done."
fi
