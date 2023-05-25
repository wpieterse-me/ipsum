#!/bin/bash

CONFIGURATION=$1
BINARY_DIRECTORY=$(pwd)/build/${CONFIGURATION}

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
