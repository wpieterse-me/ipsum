#!/bin/bash

export CONFIGURATION=$1
export BINARY_DIRECTORY=$(pwd)/build/${CONFIGURATION}

clear

echo "************"
echo "* GENERATE *"
echo "************"
cmake --preset ${CONFIGURATION}
echo ""

echo "*********"
echo "* BUILD *"
echo "*********"
cmake --build --preset ${CONFIGURATION}
echo ""

echo "*******"
echo "* RUN *"
echo "*******"
${BINARY_DIRECTORY}/bench
