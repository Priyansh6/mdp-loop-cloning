import argparse
import os
from pathlib import Path
import subprocess

perf_run_dir = "coremark-perf-runs"


def open_report(input):
    root_path = os.path.dirname(os.path.realpath(__file__)) + "/.."
    perf_run_path = os.path.join(root_path, perf_run_dir)

    cmd = [
        "perf",
        "report",
        f"--input={perf_run_path}/{input}",
        "-g",
    ]
    subprocess.run(cmd)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        formatter_class=argparse.ArgumentDefaultsHelpFormatter
    )
    parser.add_argument(
        "-i",
        "--input",
        type=str,
        default="perf.data",
        help="input file name to open the report",
    )

    args = parser.parse_args()

    open_report(input=args.input)
