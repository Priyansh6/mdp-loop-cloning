import argparse
import os
import subprocess

sim_result_dir_prefix = "sim-result-"
progs = ['SimpleSS']
rel_types = ['debug', 'release-debuginfo', 'release']
def_rel_type = rel_types[1]

def run_simulation(program, outdir, rel_type, cpu_type, options):
    root_path = os.path.dirname(os.path.realpath(__file__)) + "/.."
    # Gem5 command
    cmd = [f"{root_path}/gem5/build/ARM/gem5.opt"]
    if outdir:
        cmd.append(f"--outdir={sim_result_dir_prefix}{outdir}")
    # Gem5 config command
    cmd.extend([
        f"{root_path}/gem5/configs/deprecated/example/se.py",
        f"--cpu-type={cpu_type}",
        "--caches",
        "-c", f"{root_path}/mdp-loop-cloning/out/build/unixlike-armv8-a-clang-{rel_type}/{program}",
    ])
    if options:
        cmd.append(f"--options={options}")
    subprocess.run(cmd)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('prog', type=str, choices=progs, help='program to run simulation on')
    parser.add_argument('--outdir', type=str, default='latest', help=f'folder to store simulation results, will be prefixed with "{sim_result_dir_prefix}"')
    parser.add_argument('-r', '--rel-type', choices=rel_types, type=str, default=def_rel_type, help='release configuration of project to run simulation on')
    parser.add_argument('--cpu-type', type=str, default='DerivO3CPU', help='type of CPU to model')
    parser.add_argument('--options', type=str, help='command line options to pass to the binary')
    args = parser.parse_args()

    run_simulation(program=args.prog, outdir=args.outdir, rel_type=args.rel_type, cpu_type=args.cpu_type, options=args.options)