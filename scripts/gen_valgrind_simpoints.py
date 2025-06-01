import os
from pathlib import Path
import subprocess
from subprocess import Popen

spec = ["600.perlbench_s", "602.gcc_s", "605.mcf_s",
        "620.omnetpp_s", "623.xalancbmk_s", "625.x264_s",
        "631.deepsjeng_s", "641.leela_s", "657.xz_s"]
spec_path = os.path.dirname(os.path.realpath(__file__)) + "/../benchmarks/spec2017-x86/"
simpoint = os.path.dirname(os.path.realpath(__file__)) + "/../SimPoint/bin/simpoint"
simpoint_dir = "/media/priyansh/DRIVE/simpoints-x86/"
checkpoint_dir = "/media/priyansh/DRIVE/checkpoints-expanded-x86/"
procs = []
bench_names = []
label = "x86WithLateLoopClone"

def run_test():
    for bench in spec:
        run_dir = f"{spec_path}benchspec/CPU/{bench}/run/run_peak_refspeed_{label}-64.0000"
        os.chdir(run_dir)
        specinvoke = subprocess.run([spec_path+"bin/specinvoke", "-n"], stdout=subprocess.PIPE)
        commands = [line.decode().strip() for line in specinvoke.stdout.split(b"\n") if line.startswith(b".")]
        for c, command in enumerate(commands):
            command = command.split('>')[0]
            bench_name = (bench,str(c))
            bench_names.append(bench_name)
run_test()

for bench,workload in bench_names:
    out_dir = simpoint_dir+label+"/"
    Path(out_dir).mkdir(parents=True, exist_ok=True)
    subprocess.run([simpoint, "-loadFVFile",  checkpoint_dir+bench+"/"+label+"/bbvs."+workload,
            "-k", "search", "-maxK", "10", "-saveSimpoints",
            out_dir+bench+"."+workload+".simpts", "-saveSimpointWeights",
            out_dir+bench+"."+workload+".weights"], check=True)
