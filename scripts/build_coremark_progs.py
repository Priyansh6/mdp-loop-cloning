import os
from pathlib import Path
import shutil
import subprocess


def build_coremark_progs():
    coremark_path = (
        os.path.dirname(os.path.realpath(__file__)) + "/../benchmarks/coremark-pro"
    )
    os.chdir(coremark_path)
    shutil.rmtree("builds", ignore_errors=True)
    build_cmd = ["make", "TARGET=linux64", "-j", "16"]
    subprocess.run(build_cmd)


if __name__ == "__main__":
    build_coremark_progs()
