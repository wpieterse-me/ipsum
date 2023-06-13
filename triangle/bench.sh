#!/bin/bash

clear

bazel build -c opt //triangle:bench

sudo cpupower frequency-set --governor performance
taskset 0x1 ./bazel-bin/triangle/bench
sudo cpupower frequency-set --governor powersave