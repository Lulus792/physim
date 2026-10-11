"""Real blocked/closed pipes: durable progress, reliable frames and bounded queues."""
import json
import os
from pathlib import Path
import queue
import struct
import subprocess
import sys
import threading
import time

runner,module,checker,output_probe,directory=sys.argv[1:]
work=Path(directory)
class Stream:
    def __init__(self,args,root):
        self.child=subprocess.Popen(args,cwd=root,stdin=subprocess.PIPE,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
        self.gate=threading.Event();self.gate.set();self.reader_paused=threading.Event();self.chunks=queue.Queue()
        self.incoming=bytearray();self.received=[];self.logs=[];self.sent=0;self.last_time=-1
        self.thread=threading.Thread(target=self.read_stream,daemon=True);self.thread.start()
    def read_stream(self):
        try:
            while True:
                if not self.gate.is_set():self.reader_paused.set()
                self.gate.wait();self.reader_paused.clear()
                chunk=os.read(self.child.stdout.fileno(),65536);self.chunks.put(chunk)
                if not chunk:return
        except (OSError,ValueError) as error:self.chunks.put(error)
    def send(self,kind,payload=b''):
        self.child.stdin.write(struct.pack('<IIIII',0x5053494d,5,kind,len(payload),self.sent)+payload)
        self.child.stdin.flush();self.sent+=1
    def frame(self,deadline):
        while time.monotonic()<deadline:
            if len(self.incoming)>=20:
                magic,version,kind,size,sequence=struct.unpack_from('<IIIII',self.incoming)
                assert (magic,version,sequence)==(0x5053494d,5,len(self.received)) and size<=8192
                if len(self.incoming)>=20+size:
                    payload=bytes(self.incoming[20:20+size]);del self.incoming[:20+size];self.received.append(kind)
                    if kind==11:
                        level,t=struct.unpack_from('<Id',payload);self.logs.append(dict(level=level,time_s=t,message=payload[12:].decode()))
                    if kind==6:
                        t,count,objects,paused,points=struct.unpack_from('<dIIII',payload)
                        assert (count,objects,points)==(2,32,96) and paused in (0,1) and len(payload)==7976
                        first,second=struct.unpack_from('<dd',payload,24)
                        assert t==first*.001 and second==2*first and t>=self.last_time
                        self.last_time=t
                        for index in range(32):
                            x,y,z=struct.unpack_from('<ddd',payload,40+index*176+8)
                            assert (x,y,z)==(index,first,0),'Mutable or inconsistent snapshot object'
                        for index in range(96):
                            assert struct.unpack_from('<ddd',payload,5672+index*24)==(index,0,0)
                    return kind,payload
            try:chunk=self.chunks.get(timeout=.1)
            except queue.Empty:continue
            assert isinstance(chunk,bytes) and chunk,(self.received,self.child.poll(),chunk)
            self.incoming.extend(chunk)
        raise AssertionError('Timed out waiting for a frame')
    def initialize(self):
        deadline=time.monotonic()+15
        while True:
            kind,payload=self.frame(deadline)
            if kind==1:self.send(1,struct.pack('<I',3))
            if kind==6:
                assert struct.unpack_from('<d',payload)[0]==0 and struct.unpack_from('<I',payload,16)[0]==1
                return
    def close(self):
        self.gate.set()
        if self.child.poll() is None:self.child.kill()
        self.child.wait(timeout=5);self.thread.join(timeout=1)
        for pipe in (self.child.stdin,self.child.stdout,self.child.stderr):pipe.close()

def saved(path):return [json.loads(line) for line in Path(str(path)+'.pslog').read_text().splitlines()]
def probe(mode):
    root=work/mode;root.mkdir();path=root/'blocked.psrun'
    target=400 if mode=='realtime-blocked' else 1200
    speed='.25' if mode=='realtime-blocked' else '0'
    stream=Stream([runner,module,str(path),'--interactive','--speed',speed,'--dt','.001',
        '--break-at-steps',str(400 if mode=='realtime-blocked' else 1200),'--log-events'],root)
    try:
        stream.initialize();stream.gate.clear();stream.send(2)
        if mode=='closed-reader':
            assert stream.reader_paused.wait(2)
            stream.child.stdout.close();started=time.monotonic()
            assert stream.child.wait(timeout=5)==7
            assert time.monotonic()-started<4
            subprocess.run([checker,str(path),'0','recovered'],check=True,timeout=5)
            print('Closed reader: explicit failure and recoverable durable samples passed',flush=True);return
        time.sleep(4)
        progress=int((root/'backpressure-progress.txt').read_text()) if (root/'backpressure-progress.txt').exists() else 0
        print(mode,'durable progress with stopped stdout consumption:',progress,flush=True)
        assert progress>=target-target%32,progress
        subprocess.run([checker,str(path),str(target),'live'],check=True,timeout=5)
        if mode=='permanently-blocked':
            stream.send(5);started=time.monotonic()
            assert stream.child.wait(timeout=5)==7 and time.monotonic()-started<4
            subprocess.run([checker,str(path),'1200','closed'],check=True,timeout=5)
            assert saved(path)[-1]['message']=='Destroyed backpressure probe'
            print('Permanently blocked reader: bounded exit and complete file passed',flush=True);return
        stream.send(3);stream.send(4);stream.gate.set();paused=[];stopped=False
        while True:
            kind,payload=stream.frame(time.monotonic()+10)
            if kind==6 and struct.unpack_from('<I',payload,16)[0]:
                t=struct.unpack_from('<d',payload)[0];paused.append(t)
                if t==(target+1)*.001 and not stopped:stream.send(5);stopped=True
            if kind==8:break
        assert stream.child.wait(timeout=5)==0 and not stream.child.stderr.read()
        assert paused==[target*.001,target*.001,(target+1)*.001],paused
        assert stream.logs==saved(path) and [r['message'] for r in stream.logs]==[
            'Created backpressure probe','Warning 64','Warning 128',f'Step breakpoint reached: {target}','Destroyed backpressure probe']
        subprocess.run([checker,str(path),str(target+1),'closed'],check=True,timeout=5)
        print(f'Resumed reader: all {target+2} durable samples, logs, controls, BYE and contiguous sequences passed',flush=True)
    finally:stream.close()

for mode in ('resumed','realtime-blocked','permanently-blocked','closed-reader'):probe(mode)

for mode in ('coalesce','overflow'):
    root=work/mode;root.mkdir()
    # Deliberately no initial consumer: the producer must finish enqueueing first.
    child=subprocess.Popen([output_probe,mode],cwd=root,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
    try:
        deadline=time.monotonic()+3
        while not (root/'output-ready.txt').exists() and time.monotonic()<deadline:time.sleep(.005)
        count,ok=map(int,(root/'output-ready.txt').read_text().split());assert ok==1
        if mode=='overflow':
            assert child.wait(timeout=5)==7 and 900<=count<1100
        else:
            raw=child.communicate(timeout=5)[0];assert child.returncode==0
            at=sequence=0;messages=[];previews=[]
            while at<len(raw):
                magic,version,kind,size,serial=struct.unpack_from('<IIIII',raw,at)
                assert (magic,version,serial)==(0x5053494d,5,sequence) and at+20+size<=len(raw)
                payload=raw[at+20:at+20+size];messages.append((kind,payload))
                if kind==6:previews.append(struct.unpack_from('<I',payload)[0])
                at+=20+size;sequence+=1
            assert [payload for kind,payload in messages if kind==7]==[b'before',b'barrier']
            assert previews[-1]==1000 and len(previews)<100
            barrier=next(i for i,item in enumerate(messages) if item==(7,b'barrier'))
            assert all(struct.unpack_from('<I',payload)[0]==0 for kind,payload in messages[:barrier] if kind==6)
            assert all(struct.unpack_from('<I',payload)[0]>0 for kind,payload in messages[barrier+1:] if kind==6)
        print(mode,'bounded producer, queue policy and explicit shutdown passed',flush=True)
    finally:
        if child.poll() is None:child.kill()
        child.wait(timeout=5);child.stdout.close();child.stderr.close()
