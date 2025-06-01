from pathlib import Path
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
checkpoint_dir = "/media/priyansh/DRIVE/checkpoints-expanded-x86/"
label = "x86WithEarlyLoopCloneOverlapCheck"

procs = []

def load_balance():
    while psutil.virtual_memory().percent > 70 or psutil.cpu_percent() > 95: time.sleep(60*5)

def run_test():
    for bench in spec:
        run_dir = f"{spec_path}benchspec/CPU/{bench}/run/run_peak_refspeed_{label}-64.0000"
        checkpoint_path = checkpoint_dir+bench+"/"+label+"/"
        Path(checkpoint_path).mkdir(parents=True, exist_ok=True)
        os.chdir(run_dir)
        specinvoke = subprocess.run([spec_path+"bin/specinvoke", "-n"], stdout=subprocess.PIPE)
        commands = [line.decode().strip() for line in specinvoke.stdout.split(b"\n") if line.startswith(b".")]
        for c, command in enumerate(commands):
            command = command.split('>')[0]
            bench_name = bench+"."+str(c)
            p = subprocess.Popen("valgrind --tool=exp-bbv --bb-out-file="+checkpoint_path+"bbvs."+str(c)+" "+command.split()[0]+" "+' '.join(command.split()[1:])+" 2>&1 > "+bench_name+".out 2>&1", shell=True)
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
