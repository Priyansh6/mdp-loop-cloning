import argparse
import os
from pathlib import Path
import subprocess

perf_run_dir = "coremark-perf-runs"
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


def run_simulation(program, iterations, output):
    root_path = os.path.dirname(os.path.realpath(__file__)) + "/.."
    perf_run_path = os.path.join(root_path, perf_run_dir)
    Path(perf_run_path).mkdir(parents=False, exist_ok=True)

    cmd = [
        "perf",
        "record",
        "--event=cpu-cycles:u",
        "--freq=max",
        f"--output={perf_run_path}/{output}",
        "-g",
        "--",
        "bash",
        "-c",
        f"for i in {{1..{iterations}}}; do {root_path}/benchmarks/coremark-pro/builds/linux64/gcc64/bin/{progs[program]}.exe -v0 -i1 > /dev/null; done",
    ]
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
        "-i",
        "--iterations",
        type=int,
        default=1,
        help="number of iterations to run the simulation",
    )
    parser.add_argument(
        "-o",
        "--output",
        type=str,
        default="perf.data",
        help="output file name to store the perf data",
    )
    args = parser.parse_args()

    run_simulation(program=args.prog, iterations=args.iterations, output=args.output)
