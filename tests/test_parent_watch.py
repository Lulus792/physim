"""Real callback hangs survive only while their controller's input pipe is alive."""
import argparse
import csv
import hashlib
import os
from pathlib import Path
import struct
import subprocess
import time
import zlib

parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--runner',type=Path,required=True)
parser.add_argument('--batch',type=Path,required=True)
parser.add_argument('--module',type=Path,required=True)
parser.add_argument('--work',type=Path,required=True)
args=parser.parse_args();runner=args.runner.resolve();batch=args.batch.resolve();module=args.module.resolve()
work=args.work.resolve();work.mkdir(parents=True,exist_ok=True)

def await_state(check,timeout=15):
    deadline=time.monotonic()+timeout
    while time.monotonic()<deadline:
        value=check()
        if value:return value
        time.sleep(.02)
    raise AssertionError('Expected process state did not arrive')

def active(pid):
    if os.name=='nt':
        import ctypes
        kernel=ctypes.windll.kernel32
        kernel.OpenProcess.restype=ctypes.c_void_p
        kernel.WaitForSingleObject.argtypes=[ctypes.c_void_p,ctypes.c_ulong]
        kernel.CloseHandle.argtypes=[ctypes.c_void_p]
        handle=kernel.OpenProcess(0x100000,False,pid)
        if not handle:return False
        try:return kernel.WaitForSingleObject(handle,0)==258
        finally:kernel.CloseHandle(handle)
    p=subprocess.run(['ps','-p',str(pid),'-o','stat='],capture_output=True,text=True)
    return p.returncode==0 and bool(p.stdout.strip()) and not p.stdout.strip().startswith('Z')

def live_files(directory):
    identity=directory/'identity.txt';heartbeat=directory/'heartbeat.txt'
    if identity.exists() and heartbeat.exists() and heartbeat.stat().st_size>=2:
        pid=int(identity.read_text().splitlines()[0]);assert active(pid);return pid
    return None

def stopped(pids,hearts):
    await_state(lambda:all(not active(pid) for pid in pids),10)
    sizes=[p.stat().st_size for p in hearts];time.sleep(.15)
    assert sizes==[p.stat().st_size for p in hearts],'Heartbeat continued after parent termination'

# A connected pipe keeps both create and step callbacks alive. Closing it kills
# the process with the documented setup/lifecycle exit, without a model return.
for phase in (0,1):
    directory=work/f'direct-{phase}';directory.mkdir();output=directory/'run.psrun'
    child=subprocess.Popen([str(runner),str(module),str(output),'--steps','16','--dt','.0625','--seed','0','--parent-watch','--param',f'hangPhase={phase}'],stdin=subprocess.PIPE,stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL,cwd=directory)
    try:
        pid=await_state(lambda:live_files(directory));child.stdin.write(b'not an interactive command');child.stdin.flush()
        before=(directory/'heartbeat.txt').stat().st_size
        await_state(lambda:(directory/'heartbeat.txt').stat().st_size>before)
        assert child.poll() is None
        child.stdin.close();assert child.wait(timeout=10)==125
        stopped([pid],[directory/'heartbeat.txt'])
    finally:
        if child.poll() is None:child.kill();child.wait()

# Non-pipe input and conflicting modes are rejected before user create().
for name,options,code in [('file',['--parent-watch'],125),('interactive',['--interactive','--parent-watch'],2),('duplicate',['--parent-watch','--parent-watch'],2)]:
    directory=work/name;directory.mkdir()
    p=subprocess.run([str(runner),str(module),str(directory/'run.psrun'),*options],stdin=subprocess.DEVNULL,cwd=directory,capture_output=True,timeout=10)
    assert p.returncode==code and not (directory/'identity.txt').exists(),(name,p.returncode,p.stderr)
directory=work/'describe';directory.mkdir()
p=subprocess.run([str(runner),str(module),'--describe','--parent-watch'],stdin=subprocess.PIPE,cwd=directory,capture_output=True,timeout=10)
assert p.returncode==2 and not (directory/'identity.txt').exists(),p.stderr

# Successful offline runs still complete while the parent connection stays open.
directory=work/'success';directory.mkdir()
p=subprocess.Popen([str(runner),str(module),str(directory/'run.psrun'),'--parent-watch','--steps','16','--dt','.0625','--seed','0'],stdin=subprocess.PIPE,stdout=subprocess.PIPE,stderr=subprocess.PIPE,cwd=directory)
try:
    assert p.wait(timeout=15)==0
    data=(directory/'run.psrun').read_bytes();at=16;samples=0;footer=False
    while at<len(data):
        kind,size,crc=struct.unpack_from('<III',data,at);payload=data[at+12:at+12+size];assert len(payload)==size and zlib.crc32(payload)==crc
        if kind==3:samples+=1
        if kind==4:footer=struct.unpack('<Q',payload)[0]==samples
        at+=12+size
    assert samples==17 and footer
finally:
    p.stdin.close()
    if p.poll() is None:p.kill();p.wait()

# Kill only the controller PID; workers own separate process groups. All live
# workers must stop without callback cooperation. Previously completed archives
# and the journal must remain intact, and no final report may be invented.
for workers in (1,4):
    root=work/f'series-{workers}'
    controller=subprocess.Popen([str(batch),str(runner),str(module),str(root),'position','8','16','.0625','0','--workers',str(workers),'--timeout','60',*(['--param','hangPhase=1'] if workers==4 else [])],stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL)
    pids=[];hearts=[];held=None
    try:
        if workers==1:
            first=root/'run-0001.psrun'
            await_state(lambda:(root/'completed.csv').exists() and 'run-0001.psrun' in (root/'completed.csv').read_text())
            held=hashlib.sha256(first.read_bytes()).digest()
            slots=[root/'work-0002']
        else:slots=[root/f'work-{i:04d}' for i in range(1,5)]
        for slot in slots:pids.append(await_state(lambda slot=slot:live_files(slot)));hearts.append(slot/'heartbeat.txt')
        assert len(set(pids))==len(slots) and controller.poll() is None
        controller.kill();controller.wait(timeout=10)
        stopped(pids,hearts)
        assert not (root/'summary.psreport').exists()
        if held:
            assert hashlib.sha256((root/'run-0001.psrun').read_bytes()).digest()==held
            with (root/'completed.csv').open(newline='') as file:rows=list(csv.DictReader(file))
            assert len(rows)==1 and rows[0]['file']=='run-0001.psrun'
    finally:
        if controller.poll() is None:controller.kill();controller.wait()
        for pid in pids:
            if active(pid):
                if os.name=='nt':subprocess.run(['taskkill','/F','/PID',str(pid)],capture_output=True)
                else:os.kill(pid,9)
print('Parent watchdog: connected pipes, create/step hangs, rejected input modes, complete archives and controller death with one/four workers passed')
