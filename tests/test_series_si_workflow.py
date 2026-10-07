"""Independent SI Decimal references for C and Physim owned-Series outputs."""
import argparse,csv,subprocess
from decimal import Decimal,localcontext
from pathlib import Path
p=argparse.ArgumentParser()
for name in ('c-check','language','analysis-runner','report-probe','work'):p.add_argument('--'+name,required=True)
a=p.parse_args();work=Path(a.work);work.mkdir(parents=True,exist_ok=True);c=work/'c';c.mkdir()
def run(command):
    r=subprocess.run(list(map(str,command)),capture_output=True,timeout=90)
    assert r.returncode==0,(command,r.returncode,r.stderr)
run([a.c_check,c]);prefix=work/'phys';run([a.analysis_runner,a.language,'--runs',prefix])
for path,report in [(c/'series-si.csv',c/'series-si.psreport'),(Path(str(prefix)+'-series-si.csv'),Path(str(prefix)+'.psreport'))]:
    with path.open(newline='') as f:rows=list(csv.reader(f))
    assert rows[0]==['time [s]','length [m]','combined series [m]','derivative(length) [m s^-1]','integral(length) [m s]'],rows[0]
    assert len(rows)==514
    with localcontext() as ctx:
        ctx.prec=64
        for i,row in enumerate(rows[1:]):
            t=Decimal(i)/4
            expected=[t,1+t/2,Decimal('1.5')+t/2,Decimal('.5'),Decimal('.1')+t+t*t/4]
            for actual,reference in zip(row,expected):
                assert abs(Decimal(actual)-reference)<Decimal('2e-12')*max(1,abs(reference)),(path,i,actual,reference)
    run([a.report_probe,report])
print('Series SI workflow: actual C/Physim analyses, 513 Decimal rows, canonical CSV headers, all report curve values and SI metadata passed')
