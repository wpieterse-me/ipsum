#!/usr/bin/python3

targets = [
    "wpieterse-dt",
    "skyrim",
    "morrowind",
    "valenwood",
    "public-services",
    "spare-01",
    "spare-02",
]

print("steps:")

for target in targets:
    print("  - label: \":hammer: Build ({})\"".format(target))
    print("    command: \"bazel build -c opt //...\"")
    print("    agents:")
    print("      user: \"{}\"".format(target))

print("  - wait")

for target in targets:
    print("  - label: \":hammer: Run Square Root Bench ({})\"".format(target))
    print("    command: \"bazel run -c opt //:bench\"")
    print("    agents:")
    print("      user: \"{}\"".format(target))

for target in targets:
    print("  - label: \":hammer: Run Rasterizer Bench ({})\"".format(target))
    print("    command: \"bazel run -c opt //graphics/rasterizer:bench\"")
    print("    agents:")
    print("      user: \"{}\"".format(target))
