import argparse
import os
import subprocess

def run_simulation(rel_type, cpu_type, options):
    root_path = os.path.dirname(os.path.realpath(__file__)) + "/.."
    cmd = [
        f"{root_path}/gem5/build/ARM/gem5.opt",
        f"{root_path}/gem5/configs/deprecated/example/se.py",
        f"--cpu-type={cpu_type}",
        "--caches",
        "-c", f"{root_path}/mdp-loop-cloning/out/build/unixlike-armv8-a-clang-{rel_type}/MdpLoopCloning"
    ]
    if options:
        cmd.append(f"--options={options}")
    subprocess.run(cmd)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('-r', '--rel-type', choices=['debug', 'release-debuginfo', 'release'], type=str, default='release-debuginfo', help='release configuration of project to run simulation on')
    parser.add_argument('--cpu-type', type=str, default='DerivO3CPU', help='type of CPU to model')
    parser.add_argument('-o', '--options', type=str, help='command line options to pass to the binary')
    args = parser.parse_args()

    run_simulation(args.rel_type, args.cpu_type, args.options)