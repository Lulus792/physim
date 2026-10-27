"""Read and select a complete native original through an independent UIA client."""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess
import time

from check_protocol_original import ORIGINAL


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--app', type=Path, required=True)
    parser.add_argument('--probe', type=Path, required=True)
    parser.add_argument('--work', type=Path, required=True)
    args = parser.parse_args()
    args.work.mkdir(parents=True, exist_ok=True)
    reader = args.work / 'reader'
    expected = args.work / 'expected-original.txt'
    expected.write_bytes(ORIGINAL)
    app_hash = hashlib.sha256(args.app.read_bytes()).hexdigest()
    probes = []
    with (args.work / 'app.stdout').open('wb') as out, (args.work / 'app.stderr').open('wb') as err:
        process = subprocess.Popen([str(args.app), '--workspace-state-test', str(reader),
                                    'protocol-ui-wrap-remote'], stdout=out, stderr=err)

        def wait(marker):
            deadline = time.monotonic() + 30
            while time.monotonic() < deadline:
                if marker in (args.work / 'app.stdout').read_text(encoding='utf8', errors='replace'):
                    return
                if process.poll() is not None:
                    raise RuntimeError('App stopped before '+marker)
                time.sleep(.03)
            raise RuntimeError('Timed out waiting for '+marker)

        def probe(mode, *values):
            command = [str(args.probe), str(process.pid), mode, 'Originalmeldung', *map(str, values)]
            result = subprocess.run(command, capture_output=True, timeout=30)
            row = dict(command=command, exit_code=result.returncode,
                       stdout=result.stdout.decode('utf8', errors='replace'),
                       stderr=result.stderr.decode('utf8', errors='replace'))
            probes.append(row)
            (args.work / 'probes.json').write_text(json.dumps(probes, indent=2)+'\n', encoding='utf8')
            if result.returncode:
                raise RuntimeError('Native UIA '+mode+' failed: '+row['stdout']+row['stderr'])

        try:
            wait('PROTOCOL ORIGINAL REMOTE READY')
            probe('readonly-file', expected)
            probe('scroll-text-start', 'HIER IST DAS ECHTE ENDE!')
            (reader / 'scroll-checked').write_text('ready', encoding='utf8')
            wait('PROTOCOL ORIGINAL REMOTE END')
            probe('focus')
            probe('focused-control')
            (reader / 'focus-applied').write_text('ready', encoding='utf8')
            wait('PROTOCOL ORIGINAL REMOTE SELECTED')
            probe('readonly-selected-file', expected)
            (reader / 'selected-checked').write_text('ready', encoding='utf8')
            code = process.wait(timeout=30)
            output = (args.work / 'app.stdout').read_text(encoding='utf8')
            states = [json.loads(line) for line in output.splitlines()
                      if line.startswith('{"kind":"protocol_original_remote"')]
            assert code == 0 and 'PROTOCOL ORIGINAL REMOTE SELF-TEST: PASSED' in output
            assert [row['stage'] for row in states] == list(range(3, 10))
            assert all(row['passed'] and row['bytes'] == len(ORIGINAL) for row in states)
            assert hashlib.sha256(args.app.read_bytes()).hexdigest() == app_hash
            (args.work / 'verification.json').write_text(json.dumps(dict(
                app_sha256=app_hash, original_bytes=len(ORIGINAL), independent_uia_client=True,
                full_value_and_document=True, replacement_rejected=True,
                inactive_scroll_preserved_focus_selection_caret_value=True,
                full_selected_text=True, exact_clipboard_checked_by_app=True,
                source_unchanged=True, states=states, probes=probes,
                screenshots_created=len(list(reader.glob('*.png')))), indent=2)+'\n', encoding='utf8')
        finally:
            if process.poll() is None:
                process.kill()
                process.wait()
    print('Native Original UIA: complete 64 KiB value/document/selection, readonly rejection, inactive scroll and exact clipboard passed')


if __name__ == '__main__':
    main()
