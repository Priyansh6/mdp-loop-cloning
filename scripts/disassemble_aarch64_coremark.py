import argparse
import os
import subprocess
from pathlib import Path

dissasemble_dir = "disassembled-coremark"
progs = {
    "cjpeg": "cjpeg-rose7-preset",
    "core": "core",
    "linear_alg": "linear_alg-mid-100x100-sp",
    "loops": "loops-all-mid-10k-sp",
    "nnet": "nnet_test",
    "parser": "parser-125k",
    "radix": "radix2-big-64k",
    "sha": "sha-test",
    "zip": "zip-test",
}


def dissassemble(program, out_name):
    root_path = os.path.dirname(os.path.realpath(__file__)) + "/.."
    dissasemble_path = os.path.join(root_path, dissasemble_dir)
    Path(dissasemble_dir).mkdir(parents=False, exist_ok=True)

    cmd = [
        "aarch64-linux-gnu-objdump",
        "-S",
        f"{root_path}/benchmarks/coremark-pro/builds/linux64/gcc64/bin/{progs[program]}.exe",
    ]

    with open(f"{dissasemble_path}/{out_name}.s", "w+") as file:
        subprocess.run(cmd, stdout=file)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        formatter_class=argparse.ArgumentDefaultsHelpFormatter
    )
    parser.add_argument(
        "prog", type=str, choices=progs, help="program to disassemble"
    )
    parser.add_argument(
        "-o",
        "--output-name",
        type=str,
        required=True,
        help=f'filename to store disassembly dump, stored in folder "disassembled-coremark"',
    )
    args = parser.parse_args()

    dissassemble(program=args.prog, out_name=args.output_name)
