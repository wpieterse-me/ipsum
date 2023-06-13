#!/bin/bash

clear

bazel build -c opt //graphics/rasterizer:bench

sudo cpupower frequency-set --governor performance
taskset 0x1 ./bazel-bin/graphics/rasterizer/bench
sudo cpupower frequency-set --governor powersave
