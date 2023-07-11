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
    print("  - label: \":hammer: Run Bench ({})\"".format(target))
    print("    command: \"bazel run -c opt //:bench\"")
    print("    agents:")
    print("      user: \"{}\"".format(target))
