"""Load actual modules in the runner: versioned prefix, callbacks and extensions."""
import argparse
from pathlib import Path
import struct
import subprocess
import zlib

parser=argparse.ArgumentParser()
parser.add_argument('--runner',required=True)
parser.add_argument('--work',type=Path,required=True)
parser.add_argument('--modules',nargs=17,required=True)
args=parser.parse_args()
args.work.mkdir(parents=True,exist_ok=True)

for case,module in enumerate(args.modules):
    path=args.work/f'abi-{case}.psrun'
    command=[args.runner,module,str(path),'--steps','2','--dt','.25','--record-scenes']
    if case in (10,11):command+=['--adaptive','--min-dt','1e-8','--max-dt','.25']
    result=subprocess.run(command,capture_output=True,text=True,timeout=20)
    callbacks=[line.removeprefix('ABI_CALLBACK ') for line in result.stderr.splitlines() if line.startswith('ABI_CALLBACK ')]
    if 1<=case<=12:
        assert result.returncode==4 and not path.exists() and not callbacks,(case,result)
        expected='adaptive_step callback' if case in (10,11) else 'ABI mismatch or missing callback'
        assert expected in result.stderr,(case,result.stderr)
        continue
    if case==16:
        assert result.returncode==5 and not path.exists() and callbacks==['create','destroy'],result
        assert 'Out of memory' in result.stderr,result.stderr
        continue
    assert result.returncode==0,result
    assert callbacks.count('create')==callbacks.count('reset')==callbacks.count('destroy')==1
    assert callbacks.count('step')==2 and callbacks.count('scene')>=1
    assert callbacks[0]=='create' and callbacks[-1]=='destroy'
    rows=[];schema=footer=False
    with path.open('rb') as file:
        assert file.read(16)==b'PSRUN17\n'+struct.pack('<II',1,0x01020304)
        while header:=file.read(12):
            kind,size,crc=struct.unpack('<III',header)
            data=file.read(size)
            assert size<=8192 and len(data)==size and zlib.crc32(data)==crc
            if kind==2:assert struct.unpack_from('<I',data)[0]==1;schema=True
            if kind==3:rows.append(struct.unpack('<2d',data))
            if kind==4:assert struct.unpack('<Q',data)[0]==3;footer=True
    assert schema and footer and rows==[(0,0),(.25,.25),(.5,.5)],(case,rows)
print('Experiment ABI: 17 real modules, complete current/base prefixes, future capability, 12 pre-create rejections and failed-create destruction passed')
