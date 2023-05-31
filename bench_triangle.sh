#!/bin/bash

clear

bazel build -c opt //:bench_triangle

sudo cpupower frequency-set --governor performance
taskset 0x1 ./bazel-bin/bench_triangle
sudo cpupower frequency-set --governor powersave
