"""Native protocol navigation/layout regression; physical assistive use remains separate."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import struct
import subprocess
import sys

MODES = ('protocol-ui-dark', 'protocol-ui-light', 'protocol-ui-contrast',
         'protocol-ui-large', 'protocol-ui-narrow', 'protocol-ui-boundary', 'protocol-ui-empty')


def require(condition, message):
    if not condition:
        raise RuntimeError(message)


def png_size(path):
    data = path.read_bytes()
    require(data[:8] == b'\x89PNG\r\n\x1a\n' and data[12:16] == b'IHDR', 'Invalid PNG: ' + str(path))
    return struct.unpack('>II', data[16:24])


def verify(work, mode, code, stdout, root, *, unfocused=False):
    require(code == 0, f'Native app exited {code}: {mode}')
    require(f'PROTOCOL NAVIGATOR {mode} SELF-TEST: PASSED' in stdout, 'Native completion marker missing')
    states = [json.loads(line) for line in stdout.splitlines() if line.startswith('{"kind":"protocol_ui_step"')]
    expected = list(range(3, 41 if unfocused else 73)) if mode == 'protocol-ui-dark' else [3]
    require([state['stage'] for state in states] == expected, 'Native action sequence incomplete')
    require(all(state['passed'] for state in states), 'Native action failed')
    require(len({state['source_crc'] for state in states}) == 1, 'Editor changed during navigation')
    source = (work / 'project/main.c').read_bytes()
    require(source == (root / 'examples/pendulum/main.c').read_bytes(), 'Saved example changed')
    if mode == 'protocol-ui-dark':
        stages = {state['stage']: state for state in states}
        require(stages[9]['query_bytes'] == 2 and stages[9]['rows'] == 1, 'Unicode filter not exercised')
        selection = stages[29]
        require(selection['selection_end'] > selection['selection_start'], 'Source jump selected no text')
        require(source.decode('utf-8')[:selection['selection_start']].count('\n') + 1 == 13, 'Source jump missed line 13')
        exported = (work / 'filtered.txt').read_text(encoding='utf-8')
        require('gravity_m_s' in exported and 'Modellwert α' in exported, 'TXT missing filtered originals')
        require('energy' not in exported and 'Modellannahme prüfen' not in exported, 'TXT contains excluded warnings')
        require(not (work / 'missing/protocol.txt').exists(), 'Missing-folder negative export unexpectedly succeeded')
        if unfocused:
            require(stages[40]['height'] == 340, 'Hidden window reacted to injected key')
        else:
            require(stages[40]['height'] == 380, 'Shift+Up did not move 40 logical pixels')
            require(stages[64]['detached'] is False and stages[68]['detached'] is True and stages[72]['detached'] is False,
                    'Return/reopen cycle incomplete')
            width, height = png_size(work / 'protocol-navigator.png')
            density = width / 1440
            require(abs(height / 1000 - density) < .01, 'Unexpected source capture scaling')
            require(png_size(work / 'protocol-detached.png') == (width, round(340 * density)), 'Extraction changed client size')
    geometry = [json.loads(line) for line in stdout.splitlines() if line.startswith('{"kind":"protocol_ui_geometry"')]
    require(geometry and geometry[0]['stage'] == 3, 'Initial geometry missing')
    initial = geometry[0]
    require(initial['filters'][0][2] < initial['filters'][1][2] < initial['filters'][2][2], 'Filters use stretched equal columns')
    if mode == 'protocol-ui-dark' and not unfocused:
        detached = next((g for g in geometry if g['stage'] == 61), None)
        reopened = next((g for g in geometry if g['stage'] == 68), None)
        require(detached is not None and reopened is not None, 'Extraction geometry missing')
        require(detached['popup'] == [1440, 340] and reopened['popup'] == [1440, 340], 'Client size not retained')
        offset = initial['panes'][0][1] - detached['panes'][0][1]
        for before, after in zip(initial['panes'], detached['panes']):
            require(before[0] == after[0] and before[2:] == after[2:] and abs(before[1] - after[1] - offset) < .01,
                    'Extraction reflowed protocol columns')
    return dict(mode=mode, unfocused=unfocused, steps=len(states), source_crc=states[0]['source_crc'], geometry=geometry)


def main():
    for stream in (sys.stdout, sys.stderr):
        if hasattr(stream, 'reconfigure'):
            stream.reconfigure(encoding='utf-8', errors='replace')
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--app', type=Path, required=True)
    parser.add_argument('--work', type=Path, required=True)
    parser.add_argument('--root', type=Path, required=True)
    args = parser.parse_args()
    args.app, args.work, args.root = args.app.resolve(), args.work.resolve(), args.root.resolve()
    args.work.mkdir(parents=True, exist_ok=True)
    program_hash = hashlib.sha256(args.app.read_bytes()).hexdigest()
    results = []
    for mode, unfocused in [*((m, False) for m in MODES), ('protocol-ui-dark', True)]:
        work = args.work / ('unfocused' if unfocused else mode) / 'workflow ä'
        work.mkdir(parents=True, exist_ok=False)
        environment = dict(os.environ, SDL_FORCE_RAISEWINDOW='1', PHYSIM_TEST_TRACE='1')
        environment.pop('PHYSIM_TEST_ALL_CAPTURES', None)
        environment.pop('PHYSIM_TEST_PROTOCOL_UNFOCUSED', None)
        if unfocused:
            environment['PHYSIM_TEST_PROTOCOL_UNFOCUSED'] = '1'
        command = [str(args.app), '--workspace-state-test', str(work), mode]
        try:
            completed = subprocess.run(command, cwd=args.work, env=environment, capture_output=True,
                                       timeout=90 if mode == 'protocol-ui-dark' else 45)
            code, stdout, stderr = completed.returncode, completed.stdout, completed.stderr
        except subprocess.TimeoutExpired as error:
            code, stdout, stderr = None, error.stdout or b'', error.stderr or b''
        (work / 'stdout.bin').write_bytes(stdout)
        (work / 'stderr.bin').write_bytes(stderr)
        record = dict(command=command, exitcode=code, timed_out=code is None, passed=False,
                      stdout_sha256=hashlib.sha256(stdout).hexdigest(), stderr_sha256=hashlib.sha256(stderr).hexdigest())
        try:
            record.update(verify(work, mode, code, stdout.decode('utf-8', errors='backslashreplace'), args.root,
                                 unfocused=unfocused), passed=True)
        except (RuntimeError, OSError, ValueError) as error:
            record['error'] = str(error)
        results.append(record)
        (args.work / 'verification.json').write_text(json.dumps(dict(binary_sha256=program_hash, cases=results), indent=2)+'\n', encoding='utf-8')
        require(record['passed'], record.get('error', 'Native protocol verification failed'))
        require(hashlib.sha256(args.app.read_bytes()).hexdigest() == program_hash, 'Native binary changed during verification')
    print('Native protocol navigator: 8 states/workflows, source integrity, filters, TXT, focus and extraction passed')


if __name__ == '__main__':
    main()
