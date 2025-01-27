import argparse
import os
import subprocess
from pathlib import Path

dissasemble_dir = "disassembled"
progs = ['SimpleSS']
rel_types = ['debug', 'release-debuginfo', 'release']
def_rel_type = rel_types[1]

def run_simulation(program, out_name, rel_type):
    root_path = os.path.dirname(os.path.realpath(__file__)) + "/.."
    dissasemble_path = os.path.join(root_path, dissasemble_dir)
    Path(dissasemble_dir).mkdir(parents=False, exist_ok=True)

    cmd = ["aarch64-linux-gnu-objdump"]
    if rel_type == 'debug' or rel_type == 'release-debuginfo':
        cmd.append("-S")
    elif rel_type == 'release':
        cmd.append("-d")
    cmd.append(f"{root_path}/mdp-loop-cloning/out/build/unixlike-armv8-a-clang-{rel_type}/{program}")

    with open(f"{dissasemble_path}/{out_name}.s",'w+') as file:
        subprocess.run(cmd, stdout=file)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('prog', type=str, choices=progs, help='program to run simulation on')
    parser.add_argument('-o', '--output-name', type=str, required=True, help=f'filename to store disassembly dump, stored in folder "disassembled"')
    parser.add_argument('-r', '--rel-type', choices=rel_types, type=str, default=def_rel_type, help='release configuration of project to run disassembly on')
    args = parser.parse_args()

    run_simulation(program=args.prog, out_name=args.output_name, rel_type=args.rel_type)