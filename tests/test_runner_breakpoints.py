"""Real C/Physim runners: exact accepted-step stops, resume and unchanged data."""
import json
import os
from pathlib import Path
import queue
import struct
import subprocess
import sys
import threading
import time
import zlib

runner, c_module, phys_module, folder = sys.argv[1:]
work = Path(folder)
work.mkdir(parents=True, exist_ok=True)

def samples(path):
    data = path.read_bytes(); at = 16; rows = []; metadata = None; footer = False
    while at < len(data):
        kind, size, crc = struct.unpack_from('<III', data, at); at += 12
        payload = data[at:at+size]; at += size
        assert len(payload) == size and zlib.crc32(payload) == crc
        if kind == 1: metadata = payload.decode()
        if kind == 3: rows.append(struct.unpack('<'+'d'*(size//8), payload))
        if kind == 4: footer = True
    assert at == len(data) and metadata is not None
    return rows, metadata, footer

class Control:
    def __init__(self, args):
        self.child = subprocess.Popen(args, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        self.queue = queue.Queue(); self.data = bytearray(); self.sent = self.received = 0
        def read():
            try:
                while chunk := os.read(self.child.stdout.fileno(), 65536): self.queue.put(chunk)
                self.queue.put(None)
            except OSError as error: self.queue.put(error)
        self.reader = threading.Thread(target=read, daemon=True); self.reader.start()
    def send(self, kind, payload=b''):
        self.child.stdin.write(struct.pack('<5I', 0x5053494d, 5, kind, len(payload), self.sent)+payload)
        self.child.stdin.flush(); self.sent += 1
    def frame(self, deadline):
        while True:
            if len(self.data) >= 20:
                magic, version, kind, size, sequence = struct.unpack_from('<5I', self.data)
                assert (magic, version, sequence) == (0x5053494d, 5, self.received) and size <= 2000000
                if len(self.data) >= 20+size:
                    payload = bytes(self.data[20:20+size]); del self.data[:20+size]; self.received += 1
                    return kind, payload
            left = deadline-time.monotonic(); assert left > 0, 'runner frame deadline'
            chunk = self.queue.get(timeout=left)
            assert isinstance(chunk, bytes), (chunk, self.child.poll())
            self.data.extend(chunk)
    def snapshot(self, paused=True, after=-1):
        deadline = time.monotonic()+10
        while True:
            kind, payload = self.frame(deadline)
            assert kind in (6, 9, 11), kind
            if kind == 6:
                logical_time = struct.unpack_from('<d', payload)[0]
                state = struct.unpack_from('<I', payload, 16)[0]
                assert state in (0, 1)
                if bool(state) == paused and logical_time > after: return logical_time
    def close(self):
        if self.child.poll() is None: self.child.kill()
        self.child.wait(timeout=5); self.reader.join(timeout=5)
        assert not self.reader.is_alive()
        self.child.communicate(timeout=5)

for language, module in [('c', c_module), ('phys', phys_module)]:
    for adaptive in (False, True):
        prefix = f'{language}-'+('adaptive' if adaptive else 'fixed')
        output = work/(prefix+'.psrun'); reference = work/(prefix+'-reference.psrun')
        extra = ['--adaptive', '--min-dt', '1e-8', '--max-dt', '.05'] if adaptive else []
        args = [runner, module, str(output), '--interactive', '--speed', '0', '--dt', '.005',
                '--break-at-steps', '7, 0,3', *extra]
        control = Control(args)
        try:
            kind, payload = control.frame(time.monotonic()+10); assert kind == 1
            control.send(1, struct.pack('<I', 3)); assert control.snapshot() == 0
            control.send(2); assert control.snapshot() == 0  # Explicit initial-state stop.
            control.send(4); first = control.snapshot(after=0)
            for target in (3, 7):
                control.send(2); held = control.snapshot(after=first)
                rows, metadata, footer = samples(output)
                assert not footer and len(rows) == target+1 and rows[-1][0] == held
                assert 'break_at_steps=0,3,7\n' in metadata
                time.sleep(.04)
                assert samples(output)[0] == rows, 'physics advanced while held'
                first = held
            control.send(5)
            while control.frame(time.monotonic()+10)[0] != 8: pass
            assert control.child.wait(timeout=5) == 0 and not control.child.stderr.read()
        finally: control.close()
        result = subprocess.run([runner, module, str(reference), '--steps', '7', '--dt', '.005', *extra],
                                capture_output=True, timeout=20)
        assert result.returncode == 0, result.stderr
        actual, _, footer = samples(output); expected, _, expected_footer = samples(reference)
        assert footer and expected_footer and actual == expected and len(actual) == 8
        logs = [json.loads(line) for line in Path(str(output)+'.pslog').read_text().splitlines()]
        assert [row['message'] for row in logs if row['message'].startswith('Step breakpoint reached:')] == [
            'Step breakpoint reached: 0', 'Step breakpoint reached: 3', 'Step breakpoint reached: 7']

for index, text in enumerate(('3,3','-1','+1','1.5','1e2',',1','1,','1,,2','1000000001','18446744073709551615','α')):
    output = work/f'invalid-{index}.psrun'
    result = subprocess.run([runner, c_module, str(output), '--interactive', '--break-at-steps', text],
                            capture_output=True, timeout=5)
    assert result.returncode == 2 and not output.exists() and not Path(str(output)+'.pslog').exists()
output = work/'invalid-offline.psrun'
result = subprocess.run([runner, c_module, str(output), '--break-at-steps', '3'],capture_output=True,timeout=5)
assert result.returncode == 2 and not output.exists()
print('Step breakpoints: four real C/Physim fixed/adaptive runs, initial/3/7 stops, single-step/resume, exact persisted rows and twelve preflight rejections passed')
