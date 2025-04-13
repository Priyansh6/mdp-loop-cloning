import argparse
import os
import subprocess

sim_result_dir_prefix = "coremark-sim-result-"
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

def run_simulation(program, outdir, cpu_type, options):
    root_path = os.path.dirname(os.path.realpath(__file__)) + "/.."
    # Gem5 command
    cmd = [f"{root_path}/gem5/build/ARM/gem5.opt"]
    if outdir:
        cmd.append(f"--outdir={sim_result_dir_prefix}{outdir}")
    # Gem5 config command
    cmd.extend(
        [
            f"{root_path}/gem5/configs/deprecated/example/se.py",
            f"--cpu-type={cpu_type}",
            "--caches",
            "-c",
            f"{root_path}/benchmarks/coremark-pro/builds/linux64/gcc64/bin/{progs[program]}.exe",
        ]
    )
    if options:
        cmd.append(f"--options={options}")
    subprocess.run(cmd)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        formatter_class=argparse.ArgumentDefaultsHelpFormatter
    )
    parser.add_argument(
        "prog",
        type=str,
        choices=progs.keys(),
        help="coremark program to run simulation on",
    )
    parser.add_argument(
        "--outdir",
        type=str,
        default="latest",
        help=f'folder to store simulation results, will be prefixed with "{sim_result_dir_prefix}"',
    )
    parser.add_argument(
        "--cpu-type", type=str, default="DerivO3CPU", help="type of CPU to model"
    )
    parser.add_argument(
        "--options", type=str, help="command line options to pass to the binary"
    )
    args = parser.parse_args()

    run_simulation(
        program=args.prog,
        outdir=args.outdir,
        cpu_type=args.cpu_type,
        options=args.options,
    )
