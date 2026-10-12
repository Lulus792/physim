"""Actual archived module/Core/runner compiler identity and legacy ABI bounds."""
from run_archive_reference import execution_independent_bytes
from pathlib import Path
import re
import struct
import subprocess
import sys
import zlib
runner,c_module,language_module,legacy,future,*rest=sys.argv[1:];bad=rest[:-1];work=Path(rest[-1])
def inspect(path):
    raw=path.read_bytes();at=16;rows=[];meta=None
    assert raw[:16]==b'PSRUN17\n'+struct.pack('<II',1,0x01020304)
    while at<len(raw):
        kind,n,crc=struct.unpack_from('<III',raw,at);data=raw[at+12:at+12+n]
        assert len(data)==n and zlib.crc32(data)==crc
        if kind==1:meta=data.decode().splitlines()
        elif kind==3:rows.append(struct.unpack('<dd',data))
        at+=12+n
    assert at==len(raw);return meta,rows
for name,module in [('C',c_module),('Physim',language_module),('legacy',legacy),('future',future)]:
    path=work/(name+'.psrun');cmd=[runner,module,str(path),'--steps','2','--dt','.25']
    if name in ('legacy','future'):cmd+=['--adaptive','--min-dt','.001','--max-dt','.25']
    result=subprocess.run(cmd,capture_output=True,timeout=15);assert result.returncode==0,result.stderr
    lines,rows=inspect(path)
    for key in ('core_compiler','runner_compiler','module_compiler','core_build','runner_build','module_build'):
        values=[s.split('=',1)[1] for s in lines if s.startswith(key+'=')];assert len(values)==1,(key,lines)
        value=values[0]
        if name in ('legacy','future') and key.startswith('module_'):assert value=='unavailable'
        elif key.endswith('_compiler'):assert re.match(r'^(Clang|GCC|MSVC) .+',value),value
        else:assert re.fullmatch(r'[A-Z][a-z]{2} [ 0-3][0-9] [0-9]{4} [0-9]{2}:[0-9]{2}:[0-9]{2}',value),value
    assert 'api=3' in lines and rows[0][0]==0 and len(rows)==3
    if name in ('legacy','future'):assert rows==[(0,0),(.25,.25),(.5,.5)]
    # Execution UTC may differ; every other byte must remain deterministic.
    path2=work/(name+'-repeat.psrun');cmd[2]=str(path2)
    result=subprocess.run(cmd,capture_output=True,timeout=15);assert result.returncode==0
    assert execution_independent_bytes(path2.read_bytes())==execution_independent_bytes(path.read_bytes())
for index,module in enumerate(bad):
    path=work/('invalid-'+str(index)+'.psrun')
    result=subprocess.run([runner,module,str(path),'--steps','1'],capture_output=True,timeout=10)
    assert result.returncode!=0 and not path.exists(),(index,result.stderr)
print('Build origin: actual C/Physim/Core/runner compiler and build literals, legacy adaptive ABI, deterministic repeats and invalid/colliding origin non-creation passed')
