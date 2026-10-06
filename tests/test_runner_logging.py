"""Exercise real C/Phys runners, their binary stream and exclusive JSONL logs."""
import json
import math
import os
from pathlib import Path
import select
import struct
import subprocess
import sys
import time

runner, module, directory = sys.argv[1:]
work = Path(directory)

def records(path):
    raw = Path(str(path) + '.pslog').read_bytes()
    assert raw.endswith(b'\n')
    rows = [json.loads(line) for line in raw.splitlines()]
    assert all(set(row) == {'level', 'time_s', 'message'} and 1 <= row['level'] <= 4
               and math.isfinite(row['time_s']) for row in rows)
    return rows

def offline(name, *args):
    path = work / (name + '.psrun')
    result = subprocess.run([runner, module, str(path), '--steps', '2', '--dt', '.25', *args],
                            capture_output=True, timeout=15)
    assert result.returncode == 0, result.stderr
    return path

# Discovery suppresses logging and preserves its machine-readable output.
discovery = subprocess.run([runner, module, '--describe'], capture_output=True, timeout=15)
assert discovery.returncode == 0 and discovery.stdout.startswith(b'PHYSIM_PARAMETERS_')
assert not discovery.stderr and not Path('--describe.pslog').exists()
path = offline('normal')
rows = records(path)
assert rows[0] == dict(level=2, time_s=0, message='Created: α')
assert any(row['message'] == 'Reset "quoted"\t☃' and row['level'] == 1 for row in rows)
assert [(r['time_s'], r['message']) for r in rows if r['level'] == 3] == [(0, 'Step\nnext line'), (.25, 'Step\nnext line')]
if 'Destroyed' in [r['message'] for r in rows]:
    assert rows[-1] == dict(level=4, time_s=.5, message='Destroyed')

# Failed sink creation must never truncate someone else's file or alter physics.
foreign = work / 'foreign.psrun'
sidecar = Path(str(foreign) + '.pslog')
sidecar.write_bytes(b'foreign content\n')
assert offline('foreign').read_bytes() == path.read_bytes()
assert sidecar.read_bytes() == b'foreign content\n'

# JSON escaping and bounded volume are tested independently of the wire stream.
if 'burst' in discovery.stdout.decode():
    flooded = offline('flood', '--param', 'burst=5000')
    flood = records(flooded)
    assert len(flood) == 4097 and flood[-1]['level'] == 3
    assert 'suppressed 909 messages' in flood[-1]['message'], flood[-1]
    assert flood[-1]['time_s'] == .5

# Read stdout as frames, including messages before HELLO and destroy before BYE.
for enabled in (False, True):
    output = work / ('events' if enabled else 'legacy')
    args = [runner, module, str(output), '--interactive', '--dt', '.25']
    if enabled:
        args.append('--log-events')
    child = subprocess.Popen(args, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    incoming = bytearray()
    received, logged = [], []
    sent = 0
    snapshots = 0
    deadline = time.monotonic() + 15
    def send(kind, payload=b''):
        global sent
        child.stdin.write(struct.pack('<IIIII', 0x5053494d, 5, kind, len(payload), sent) + payload)
        child.stdin.flush()
        sent += 1
    try:
        while not received or received[-1] != 8:
            assert time.monotonic() < deadline, received
            ready, _, _ = select.select([child.stdout], [], [], .1)
            if ready:
                chunk = os.read(child.stdout.fileno(), 65536)
                assert chunk, (received, child.poll())
                incoming.extend(chunk)
            while len(incoming) >= 20:
                magic, version, kind, size, sequence = struct.unpack_from('<IIIII', incoming)
                assert magic == 0x5053494d and version == 5 and sequence == len(received)
                assert size <= 2000000
                if len(incoming) < 20 + size:
                    break
                payload = bytes(incoming[20:20 + size])
                del incoming[:20 + size]
                received.append(kind)
                if kind == 11:
                    level, logical_time = struct.unpack_from('<Id', payload)
                    logged.append(dict(level=level, time_s=logical_time, message=payload[12:].decode('utf-8')))
                elif kind == 1:
                    send(1, struct.pack('<I', 3))
                elif kind == 6:
                    snapshots += 1
                    send(4 if snapshots <= 2 else 5)
                else:
                    assert kind in (8, 9), kind
        assert not incoming and child.wait(timeout=5) == 0 and not child.stderr.read()
        assert snapshots == 3
        saved = records(output)
        assert logged == (saved if enabled else [])
        assert received[0] == (11 if enabled else 1)
        assert [(r['time_s'], r['message']) for r in saved if r['level'] == 3] == [(0, 'Step\nnext line'), (.25, 'Step\nnext line')]
    finally:
        if child.poll() is None:
            child.kill()
        child.communicate(timeout=5)

invalid = subprocess.run([runner, module, str(work / 'invalid'), '--log-events'], capture_output=True, timeout=5)
assert invalid.returncode == 2 and not Path(str(work / 'invalid') + '.pslog').exists()
print('Logging: C/Phys phases, JSONL escaping, persistence, exclusive files, opt-in wire sequence and unchanged physics passed')
