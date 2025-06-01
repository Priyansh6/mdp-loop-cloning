import os
from pathlib import Path
import subprocess


def build_gem5():
    gem5_path = os.path.dirname(os.path.realpath(__file__)) + "/../spec-gem5"
    build_path = f"{gem5_path}/build"
    Path(build_path).mkdir(parents=False, exist_ok=True)
    os.chdir(gem5_path)
    cmd = ["scons", "build/X86/gem5.fast", "--ignore-style", "-j 16"]
    subprocess.run(cmd)


if __name__ == "__main__":
    build_gem5()
