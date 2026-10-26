"""Check the original-text verifier against an actual native run and four defects."""
import argparse
import copy
import hashlib
import json
from pathlib import Path

from check_protocol_original import MODES, verify


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--root', type=Path, required=True)
    parser.add_argument('--work', type=Path, required=True,
                        help='Completed dark-mode workflow folder with stdout.bin and its PNG')
    args = parser.parse_args()
    raw = (args.work / 'stdout.bin').read_bytes()
    stdout = raw.decode('utf-8')
    control = verify(args.work, MODES[0], 0, stdout, args.root)
    states = [json.loads(line) for line in stdout.splitlines()
              if line.startswith('{"kind":"protocol_original_step"')]
    results = []
    for name in ('missing_copy_step', 'changed_original_crc',
                 'clipboard_mismatch', 'blank_final_glyph'):
        changed = copy.deepcopy(states)
        if name == 'missing_copy_step':
            changed = [step for step in changed if step['stage'] != 9]
        elif name == 'changed_original_crc':
            changed[-1]['original_crc'] ^= 1
        elif name == 'clipboard_mismatch':
            next(step for step in changed if step['stage'] == 9)['copy_equal'] = False
        else:
            next(step for step in changed if step['stage'] == 6)['end_box'][0] += 40
        evidence = '\n'.join(json.dumps(step, separators=(',', ':')) for step in changed)
        evidence += '\nPROTOCOL ORIGINAL ' + MODES[0] + ' SELF-TEST: PASSED\n'
        try:
            verify(args.work, MODES[0], 0, evidence, args.root)
        except RuntimeError as error:
            results.append({'mutation': name, 'rejected': True, 'reason': str(error)})
        else:
            raise RuntimeError('Defective counterexample was accepted: ' + name)
    verifier = Path(__file__).with_name('check_protocol_original.py')
    report = {'verifier_sha256': hashlib.sha256(verifier.read_bytes()).hexdigest(),
              'input_stdout_sha256': hashlib.sha256(raw).hexdigest(),
              'control': control, 'counterexamples': results, 'native_runs': False}
    (args.work / 'counterexamples.json').write_text(
        json.dumps(report, indent=2) + '\n', encoding='utf-8')
    print('Actual native control accepted; missing step, changed bytes, false copy and blank glyph rejected')


if __name__ == '__main__':
    main()
