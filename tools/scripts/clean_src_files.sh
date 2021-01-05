#! /bin/bash
# Clean up the sourcce files after libs are generated
# example:
# clean_src_files.sh [SDK_path]

if [ "$1" == "" ]; then
	BEKEN_SDK_DIR=./beken378
else
	BEKEN_SDK_DIR=$1
fi
if [ ! -d ${BEKEN_SDK_DIR} ]; then
	echo "${BEKEN_SDK_DIR}: No such directory"
	exit 1
fi
echo "SDK DIR: ${BEKEN_SDK_DIR}"


echo "clean ip lib files..."
source ${BEKEN_SDK_DIR}/ip/lib_files.sh
for sub in ${LIB_FILES}
do
	rm -f $sub
done
rm -f ${BEKEN_SDK_DIR}/ip/lib_files.sh
rm -f ${BEKEN_SDK_DIR}/ip/ip_src.mk


echo "clean ble4.x lib files..."
source ${BEKEN_SDK_DIR}/driver/ble/lib_files.sh
for sub in ${LIB_FILES}
do
	rm -f $sub
done
rm -f ${BEKEN_SDK_DIR}/driver/ble/lib_files.sh
rm -f ${BEKEN_SDK_DIR}/driver/ble/ble_src.mk


echo "clean ble5.x lib files..."
source ${BEKEN_SDK_DIR}/driver/ble_5_x_rw/lib_files.sh
for sub in ${LIB_FILES}
do
	rm -f $sub
done
rm -f ${BEKEN_SDK_DIR}/driver/ble_5_x_rw/lib_files.sh
rm -f ${BEKEN_SDK_DIR}/driver/ble_5_x_rw/ble_src.mk


echo "clean usb lib files..."
source ${BEKEN_SDK_DIR}/driver/usb/lib_files.sh
for sub in ${LIB_FILES}
do
	rm -f $sub
done
rm -f ${BEKEN_SDK_DIR}/driver/usb/lib_files.sh
rm -f ${BEKEN_SDK_DIR}/driver/usb/usb_src.mk


echo "clean sensor lib files..."
source ${BEKEN_SDK_DIR}/func/sensor/lib_files.sh
for sub in ${LIB_FILES}
do
	rm -f $sub
done
rm -f ${BEKEN_SDK_DIR}/func/sensor/lib_files.sh
rm -f ${BEKEN_SDK_DIR}/func/sensor/sensor_src.mk

echo "clean vad lib files..."
source ${BEKEN_SDK_DIR}/func/vad/lib_files.sh
for sub in ${LIB_FILES}
do
	rm -f $sub
done
rm -f ${BEKEN_SDK_DIR}/func/vad/lib_files.sh
