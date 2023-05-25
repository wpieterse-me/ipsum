#!/bin/bash

CONFIGURATION=x86_avx512_debug

./build.sh ${CONFIGURATION}

SOURCE_DIRECTORY=$(pwd)
BUILD_DIRECTORY=${SOURCE_DIRECTORY}/build/${CONFIGURATION}

echo "*******"
echo "* RUN *"
echo "*******"
echo ""

echo " --- BENCH --- "
${BUILD_DIRECTORY}/bench
