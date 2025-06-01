import subprocess
import os
import sys
import time
import psutil

spec_path = os.path.dirname(os.path.realpath(__file__)) + "/../benchmarks/spec2017-x86/"
spec = ["600.perlbench_s", "605.mcf_s",
       "625.x264_s", "631.deepsjeng_s",
       "641.leela_s", "657.xz_s",
       "620.omnetpp_s", "602.gcc_s", 
        "623.xalancbmk_s"]
gem5 = os.path.dirname(os.path.realpath(__file__)) + "/../spec-gem5/"
checkpoint_dir = "/media/priyansh/DRIVE/checkpoints-expanded-x86/"
label = "x86WithEarlyLoopCloneOverlapCheck"
simpoint_dir = f"/media/priyansh/DRIVE/simpoints-x86/{label}/"

procs = []

def load_balance():
    time.sleep(60*2)
    while psutil.virtual_memory().percent > 85 or psutil.cpu_percent() > 95: time.sleep(60*5)

def run_test():
    for bench in spec:
        run_dir = f"{spec_path}benchspec/CPU/{bench}/run/run_peak_refspeed_{label}-64.0000"
        checkpoint_path = checkpoint_dir+bench+"/"+label+"/"
        os.chdir(run_dir)
        specinvoke = subprocess.run([spec_path+"bin/specinvoke", "-n"], stdout=subprocess.PIPE)
        commands = [line.decode().strip() for line in specinvoke.stdout.split(b"\n") if line.startswith(b".")]
        for c, command in enumerate(commands):
            command = command.split('>')[0]
            bench_name = bench+"."+str(c)
            p = subprocess.Popen(gem5+"build/X86/gem5.fast --outdir="+checkpoint_path+"checkpoints."+str(c)+" "+gem5+"configs/deprecated/example/se.py --cpu-type=X86KvmCPU --take-simpoint-checkpoint="+simpoint_dir+bench_name+".simpts,"+simpoint_dir+bench_name+".weights,100000000,10000000 -c "+command.split()[0]+" --options=\""+' '.join(command.split()[1:])+"\" --mem-size=25GB 2>&1 > "+bench_name+".out 2>&1", shell=True)
            print("Running: ", p.args)
            procs.append(p)
            load_balance()

run_test()

active_procs = procs.copy()

while active_procs:
    for proc in active_procs[:]:
        code = proc.poll()
        if code is not None:
            active_procs.remove(proc)
            if code != 0:
                print("Crash: ", proc.args)
    time.sleep(60*5)
