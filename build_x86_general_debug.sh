#!/bin/bash

export BINARY_DIRECTORY=$(pwd)/build/x86_general_debug

clear

mkdir -p ${BINARY_DIRECTORY}

cmake --preset x86_general_debug
cmake --build --preset x86_general_debug

${BINARY_DIRECTORY}/bench
