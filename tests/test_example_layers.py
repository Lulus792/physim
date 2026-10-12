"""Verify real shipped-example archives, declarations and unchanged scientific data."""
import argparse
import json
from pathlib import Path
import struct
import subprocess
import zlib

KEYS = ('geometry', 'laws', 'medium', 'losses', 'numerics', 'measurement', 'sampling')


def archive(path, rows):
    raw = path.read_bytes()
    assert raw[:16] == b'PSRUN17\n' + struct.pack('<II', 1, 0x01020304)
    at = 16
    metadata = None
    schema = None
    data = []
    complete = False
    while at < len(raw):
        kind, size, crc = struct.unpack_from('<III', raw, at)
        payload = raw[at + 12:at + 12 + size]
        assert size <= 8192 and len(payload) == size and zlib.crc32(payload) == crc
        if kind == 1:
            pairs = [line.split('=', 1) for line in payload.decode('utf-8').splitlines() if '=' in line]
            assert len({key for key, _ in pairs}) == len(pairs)
            metadata = dict(pairs)
        elif kind == 2:
            assert schema is None
            schema = payload
        elif kind == 3:
            data.append(payload)
        elif kind == 4:
            assert struct.unpack('<Q', payload)[0] == len(data)
            complete = True
        at += 12 + size
    assert at == len(raw) and complete and schema is not None and len(data) == rows
    return metadata, schema, data


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('runner', type=Path)
    parser.add_argument('work', type=Path)
    parser.add_argument('catalog', type=Path)
    parser.add_argument('modules', nargs='+', type=Path)
    parser.add_argument('--scene-probe', type=Path, required=True)
    parser.add_argument('--baseline-modules', type=Path, help='Directory containing old modules named by catalog id')
    args = parser.parse_args()
    records = json.loads(args.catalog.read_text())
    assert len(records) == len(args.modules) == 44
    args.work.mkdir(parents=True, exist_ok=True)
    declarations = {}
    for record, module in zip(records, args.modules):
        steps = 0 if record['profile'] == 'diagnostic_experiment' else 20
        output = args.work / (record['id'] + '.psrun')
        options = ['--steps', str(steps), '--dt', '.001', '--seed', '42']
        result = subprocess.run([str(args.runner), str(module), str(output), *options], capture_output=True, timeout=20)
        if record['profile'] == 'scene_shapes':
            assert result.returncode == 6 and not output.exists(), (record['id'], result.stderr)
            result = subprocess.run([str(args.scene_probe), str(module)], capture_output=True, timeout=20)
            assert result.returncode == 0, result.stderr
            metadata = dict(line.split('=', 1) for line in result.stdout.decode().splitlines() if '=' in line)
            schema = data = None
        else:
            assert result.returncode == 0, (record['id'], result.stdout, result.stderr)
            metadata, schema, data = archive(output, steps + 1)
        layers = {}
        for key in KEYS:
            state, detail = metadata['layer.' + key].split('|', 1)
            assert state in ('active', 'ideal', 'omitted') and detail.strip()
            layers[key] = (state, detail)
        assert [layers[key][0] for key in KEYS] == record['states'], (record['id'], layers)
        for key, fragment in record.get('limits', {}).items():
            assert fragment in layers[key][1], (record['id'], key, fragment)
        if schema is not None:
            assert metadata['series.kind'] == 'single'
            assert metadata['module_compiler'] != 'unavailable'
        previous = declarations.setdefault(record['profile'], layers)
        assert previous == layers, (record['id'], 'C/Physim layer mismatch')
        if args.baseline_modules and schema is not None:
            old = args.baseline_modules / (record['id'] + module.suffix)
            old_output = args.work / (record['id'] + '-before.psrun')
            result = subprocess.run([str(args.runner), str(old), str(old_output), *options], capture_output=True, timeout=20)
            assert result.returncode == 0, (record['id'], result.stderr)
            _, old_schema, old_data = archive(old_output, steps + 1)
            assert schema == old_schema and data == old_data, (record['id'], 'Scientific data changed')
        parameters = None
        expected = {}
        if record['profile'] == 'pendulum_variant':
            parameters = ['--param', 'integrator=2', '--param', 'airDensity=1.225', '--param', 'sensorNoise=.02']
            expected = {'medium': 'active', 'losses': 'active', 'measurement': 'active', 'sampling': 'active'}
        elif record['profile'] == 'collision_main':
            parameters = ['--param', 'restitution=.5']
            expected = {'losses': 'active'}
        if parameters:
            variant = args.work / (record['id'] + '-variant.psrun')
            result = subprocess.run([str(args.runner), str(module), str(variant), *options, *parameters], capture_output=True, timeout=20)
            assert result.returncode == 0, result.stderr
            meta, variant_schema, variant_data = archive(variant, steps + 1)
            for key, state in expected.items():
                assert meta['layer.' + key].split('|', 1)[0] == state, (record['id'], key)
            if args.baseline_modules:
                old_output = args.work / (record['id'] + '-variant-before.psrun')
                result = subprocess.run([str(args.runner), str(old), str(old_output), *options, *parameters], capture_output=True, timeout=20)
                assert result.returncode == 0, result.stderr
                _, old_schema, old_data = archive(old_output, steps + 1)
                assert variant_schema == old_schema and variant_data == old_data, record['id']
    print('Example layers: 49 complete CRC-verified archives and one actual channel-free scene module; mechanisms, limits, parameter variants and matching C/Physim declarations passed')
    if args.baseline_modules:
        print('Frozen predecessor: all 49 measurement schemas and sample payloads remain byte-identical')


if __name__ == '__main__':
    main()
