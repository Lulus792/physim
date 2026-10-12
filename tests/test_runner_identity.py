"""Actual C/Physim runner identities and invalid-option non-creation."""
from pathlib import Path
import struct
import subprocess
import sys
import zlib
runner,c_module,language_module,collision_module,folder=sys.argv[1:];work=Path(folder)
project='0123456789abcdef0123456789abcdef';experiment='fedcba9876543210fedcba9876543210'
def metadata(path):
    raw=path.read_bytes();assert raw[:16]==b'PSRUN17\n'+struct.pack('<II',1,0x01020304)
    kind,n,crc=struct.unpack_from('<III',raw,16);data=raw[28:28+n]
    assert kind==1 and zlib.crc32(data)==crc
    return data.decode('utf-8').splitlines()
for name,module in [('C',c_module),('Physim',language_module)]:
    for supplied in (True,False):
        path=work/(name+str(supplied)+'.psrun')
        args=[runner,module,str(path),'--steps','2']
        if supplied:args+=['--project-id',project,'--experiment-id',experiment]
        result=subprocess.run(args,capture_output=True,timeout=15)
        assert result.returncode==0,result.stderr
        lines=metadata(path)
        for key,value in [('project_id',project),('experiment_id',experiment)]:
            assert [l for l in lines if l.startswith(key+'=')]==[key+'='+(value if supplied else 'unavailable')]
    for i,value in enumerate(['','../relative','123',project.upper(),project+'0',project+'\nforged=1','α'*32]):
        path=work/(name+'bad'+str(i)+'.psrun')
        result=subprocess.run([runner,module,str(path),'--steps','1','--project-id',value],capture_output=True,timeout=5)
        assert result.returncode==2 and not path.exists() and not Path(str(path)+'.pslog').exists()
    path=work/(name+'duplicate.psrun')
    result=subprocess.run([runner,module,str(path),'--steps','1','--experiment-id',experiment,'--experiment-id',experiment],capture_output=True,timeout=5)
    assert result.returncode==2 and not path.exists()

for seed in (42,43):
    path=work/('collision'+str(seed)+'.psrun')
    result=subprocess.run([runner,collision_module,str(path),'--steps','1','--seed',str(seed),'--project-id',project],capture_output=True,timeout=5)
    assert result.returncode==5 and b'host-owned identity fields' in result.stderr and not path.exists()

print('Runner identities: exact C/Physim metadata, explicit legacy unavailability and invalid/duplicate options create no output')
