"""Run complete C/Physim Batch documentation with both experiment languages."""
from pathlib import Path
import hashlib
import subprocess
import sys
analysis,c_analysis,phys_analysis,c_model,phys_model,probe,directory=sys.argv[1:]
work=Path(directory);work.mkdir(parents=True,exist_ok=True)
def command(argv):
    p=subprocess.run(list(map(str,argv)),capture_output=True,timeout=180)
    assert p.returncode==0,(argv,p.returncode,p.stdout,p.stderr)
for m,model in enumerate((c_model,phys_model)):
    for a,module in enumerate((c_analysis,phys_analysis)):
        prefix=work/f'batch-{m}-{a}'
        command([analysis,module,'--runs',prefix,model])
        series=Path(str(prefix)+'-series')
        command([probe,Path(str(prefix)+'.psreport'),'--demo',series])
        assert len(list(series.glob('run-*.psrun')))==256
        fingerprints=[hashlib.sha256((series/f'run-{i:04d}.psrun').read_bytes()).digest() for i in range(1,257)]
        if a==0:first=fingerprints
        else:assert first==fingerprints
print('Batch documentation: four complete C/Physim combinations, 1024 immutable archives, independent reports and equal trajectories passed')
