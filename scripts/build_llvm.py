import os
from pathlib import Path
import subprocess


def build_llvm():
    llvm_path = os.path.dirname(os.path.realpath(__file__)) + "/../llvm"
    os.chdir(llvm_path)
    config_cmd = [
        "cmake",
        "-B",
        "build",
        "-S",
        "llvm",
        "-DCMAKE_BUILD_TYPE=MinSizeRel",
        "-DLLVM_ENABLE_PROJECTS='clang'",
        "-DLLVM_TARGETS_TO_BUILD=AArch64",
        "-DLLVM_ALIASHINTSPASS_LINK_INTO_TOOLS=ON",
        "-DLLVM_LOOPCLONEPASS_LINK_INTO_TOOLS=ON",
    ]
    subprocess.run(config_cmd)

    build_cmd = ["cmake", "--build", "build", "-j", "16"]
    subprocess.run(build_cmd)


if __name__ == "__main__":
    build_llvm()
