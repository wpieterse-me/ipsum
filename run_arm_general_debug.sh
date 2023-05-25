#!/bin/bash

CONFIGURATION=arm_general_debug

./build.sh ${CONFIGURATION}

SOURCE_DIRECTORY=$(pwd)
BUILD_DIRECTORY=${SOURCE_DIRECTORY}/build/${CONFIGURATION}
ARCHITECTURE=$(uname -i)

if [[ $ARCHITECTURE == x86_64* ]];
then
    EMULATOR="qemu-aarch64-static -L /usr/aarch64-linux-gnu/"
else
    EMULATOR=""
fi

echo "*******"
echo "* RUN *"
echo "*******"
echo ""

echo " --- BENCH --- "
${EMULATOR} ${BUILD_DIRECTORY}/bench
