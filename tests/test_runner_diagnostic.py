"""Validate typed failures in real C/Phys runners, independent of text parsing."""
from pathlib import Path
import queue
import struct
import subprocess
import sys
import threading
import time
import zlib

runner, c_module, phys_module, analysis_runner, c_analysis, phys_analysis, legacy_analysis, directory = sys.argv[1:]
work = Path(directory)

def decode(data):
    assert 40 <= len(data) <= 2216
    magic, version, code, line, column, *lengths = struct.unpack_from('<9I', data)
    assert magic == 0x47445350 and version == 1 and code in (1, 2, 3, 4, 5, 8, 9, 10)
    assert lengths[0] <= 64 and lengths[1] <= 64 and lengths[2] <= 1024 and lengths[3] <= 1024
    assert len(data) == 40 + sum(lengths) and zlib.crc32(data[:-4]) == struct.unpack_from('<I', data, len(data)-4)[0]
    fields = []
    at = 36
    for length in lengths:
        raw = data[at:at+length]
        assert b'\0' not in raw
        fields.append(raw.decode('utf-8'))
        at += length
    return dict(code=code, line=line, column=column, operation=fields[0], argument=fields[1], source=fields[2], message=fields[3])

scene_output = work / 'scene-failure.psrun'
run = subprocess.run([runner, c_module, str(scene_output), '--steps', '2', '--record-scenes', '--param', 'scene=1'],
                     capture_output=True, timeout=15)
assert run.returncode == 7
assert decode(Path(str(scene_output)+'.psdiag').read_bytes())['code'] == 8

for mode, module in (('c', c_module), ('phys', phys_module)):
    output = work / (mode + '-offline.psrun')
    run = subprocess.run([runner, module, str(output), '--steps', '2'], capture_output=True, timeout=15)
    assert run.returncode == 7 and b'Singular matrix' in run.stderr
    record = decode(Path(str(output)+'.psdiag').read_bytes())
    assert record['code'] == 8 and record['operation'] == 'solve' and record['argument'] == 'matrix'
    assert record['line'] > 0 and record['column'] > 0 and record['message'] == 'Singular matrix α\nChoose independent equations'
    foreign = work / (mode + '-foreign.psrun')
    sidecar = Path(str(foreign)+'.psdiag')
    sidecar.write_bytes(b'foreign diagnostic\n')
    run = subprocess.run([runner, module, str(foreign), '--steps', '2'], capture_output=True, timeout=15)
    assert run.returncode == 7 and sidecar.read_bytes() == b'foreign diagnostic\n'
    for enabled in (False, True):
        output = work / (mode + ('-typed.psrun' if enabled else '-legacy.psrun'))
        args = [runner, module, str(output), '--interactive']
        if enabled:
            args.append('--diagnostics')
        child = subprocess.Popen(args, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        chunks = queue.Queue()
        def read_stdout():
            while True:
                data = child.stdout.read1(65536)
                chunks.put(data)
                if not data:
                    return
        reader = threading.Thread(target=read_stdout, daemon=True)
        reader.start()
        buffer = bytearray()
        received = []
        error = None
        sent = 0
        deadline = time.monotonic()+15
        def send(kind, payload=b''):
            global sent
            child.stdin.write(struct.pack('<5I',0x5053494d,5,kind,len(payload),sent)+payload)
            child.stdin.flush()
            sent += 1
        try:
            while not received or received[-1] != 8:
                assert time.monotonic() < deadline, received
                try:
                    data = chunks.get(timeout=.1)
                    assert data, (received, child.poll())
                    buffer.extend(data)
                except queue.Empty:
                    pass
                while len(buffer) >= 20:
                    magic, version, kind, size, sequence = struct.unpack_from('<5I', buffer)
                    assert magic == 0x5053494d and version == 5 and sequence == len(received) and size <= 8192
                    if len(buffer) < 20+size:
                        break
                    payload = bytes(buffer[20:20+size])
                    del buffer[:20+size]
                    received.append(kind)
                    if kind == 1:
                        send(1, struct.pack('<I',3))
                    elif kind == 6:
                        send(4)
                    elif kind == 12:
                        assert enabled
                        error = decode(payload)
                    elif kind == 7:
                        assert not enabled and b'Singular matrix' in payload
                    else:
                        assert kind in (8,9), kind
            assert child.wait(timeout=5) == 7 and not child.stderr.read() and not buffer
            saved = decode(Path(str(output)+'.psdiag').read_bytes())
            assert saved == record and (error == saved if enabled else error is None)
            assert received.count(12 if enabled else 7) == 1
        finally:
            if child.poll() is None:
                child.kill()
            child.wait(timeout=5)
            reader.join(timeout=5)
            child.communicate(timeout=5)

for mode, module in (('c',c_analysis),('phys',phys_analysis)):
    output=work/(mode+'-analysis')
    run=subprocess.run([analysis_runner,module,'--runs',str(output)],capture_output=True,timeout=15)
    assert run.returncode==5, run.stderr
    record=decode(Path(str(output)+'.psdiag').read_bytes())
    assert record['code']==1 and record['operation']=='analyze' and record['argument']=='input'
    assert record['message']=='Deliberate analysis error α' and record['line']>0

output=work/'legacy-analysis'
run=subprocess.run([analysis_runner,legacy_analysis,'--runs',str(output)],capture_output=True,timeout=15)
assert run.returncode==5 and b"one run only" not in run.stderr,(run.returncode,run.stderr)
record=decode(Path(str(output)+'.psdiag').read_bytes())
assert record['code']==1 and record['operation']=='analyze' and record['source']==''

print('Typed diagnostics: C/Phys failures, exact fields, CRC persistence, exclusive files, default/opt-in wire clients and analysis callbacks passed')
