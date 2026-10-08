"""Exact analytic amplitude envelopes through real C/Physim analysis modules."""
import csv
import math
from pathlib import Path
import subprocess
import sys

fixture, runner, c_doc, phys, c_general, probe, root = map(Path, sys.argv[1:])
root.mkdir(parents=True, exist_ok=True)
def run(*args):
    r = subprocess.run(list(map(str,args)), capture_output=True, timeout=90)
    assert r.returncode == 0, (args, r.returncode, r.stdout, r.stderr)
run(fixture, root)
cases = [("decay",.1,.5,.125,400),("growth",-.1,.5,.125,400),
         ("irregular",.1,.7,.1,400),("plateau",.1,1,.3,400),
         ("rest",0,0,0,0),("single",0,0,.1,1)]
for name, rate, period, first, count in cases:
    for language, module in [("c",c_doc),("phys",phys),("general",c_general)]:
        prefix = root/(name+"-"+language)
        run(runner,module,root/(name+".psrun"),prefix)
        run(probe,str(prefix)+".psreport",rate,period,max(0,count-1))
        peaks = list(csv.reader(open(str(prefix)+"-peaks_1.csv",newline="",encoding="utf-8")))
        decay = list(csv.reader(open(str(prefix)+"-decay_1.csv",newline="",encoding="utf-8")))
        assert len(peaks) == count+1 and len(decay) == max(0,count-1)+1
        for i, row in enumerate(peaks[1:]):
            t,a,segment = map(float,row)
            assert abs(t-(first+i*period))<1e-12 and segment==0
            expected = .5 if name=="single" else math.exp(-rate*t)
            assert abs(a-expected)<1e-12*max(1,abs(expected))
        for i,row in enumerate(decay[1:]):
            start,end,a,b,decrement,observed,segment = map(float,row)
            assert abs(start-(first+i*period))<1e-12 and abs(end-start-period)<1e-12
            assert abs(decrement-rate*period)<2e-12 and abs(observed-rate)<2e-12 and segment==0
            assert a>0 and b>0 and all(math.isfinite(x) for x in (start,end,a,b,decrement,observed))
print("Decay analyses: 18 reports, 400-peak full CSVs, growth, irregular grids, plateaus and unavailable states passed")
