import os
import psutil
import time

spec_dir = os.path.dirname(os.path.realpath(__file__)) + "/../benchmarks/spec2017-x86"
bench_dir = spec_dir + "/benchspec/CPU"

benches = [
    "600.perlbench_s", 
    "602.gcc_s",
    "605.mcf_s",
    "620.omnetpp_s",
    "623.xalancbmk_s",
    "625.x264_s",
    "631.deepsjeng_s",
    "641.leela_s",
    "657.xz_s", 
    ] #"638.imagick_s"]


processes = []

os.chdir(spec_dir)
for bench in benches:
    while psutil.virtual_memory().percent > 60 and psutil.cpu_percent() > 90: time.sleep(30)
    p = psutil.Popen(["./bin/runcpu", "--action", "runsetup", "--rebuild", "--config", "x86config", "--tune", "peak", bench])
    processes.append(p)

for p in processes:
    p.wait()
    
print("Completed compiling benchmarks.....")