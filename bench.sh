#!/bin/bash

clear

sudo cpupower frequency-set --governor performance
taskset 0x1 ./bazel-bin/bench
sudo cpupower frequency-set --governor powersave
