"""Exercise package boundaries, file integrity and incomplete-harness rejection."""
import argparse
import hashlib
import importlib.util
import json
from pathlib import Path
import shutil
import tarfile
import tempfile

parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--work',type=Path,required=True)
args=parser.parse_args();args.work.mkdir(parents=True,exist_ok=True)
repo=Path(__file__).resolve().parent.parent
spec=importlib.util.spec_from_file_location('verification_package',repo/'tools/package-verification-kit.py')
kit=importlib.util.module_from_spec(spec);spec.loader.exec_module(kit)
root=Path(tempfile.mkdtemp(prefix='verification kit ä ',dir=args.work)).resolve()
archive=root/'kit.tar.gz';names=kit.package(repo,archive)
assert {'tools/sdk_series_probe.c','tests/test_saved_run_tutorial.py','tests/test_allocator.h','tests/test_pendulum_sdk.py','tests/test_positive_peaks.c',
        'tests/create_decay_fixtures.c','tests/test_pendulum_decay.py','tests/test_runner_breakpoints.py','tools/archive_bitmaps.py',
        'tests/test_language_run_stream.c','tests/check_language_stream_host.c',
        'tests/fixtures/language/run_stream.phys','tests/fixtures/language/run_stream_experiment.phys',
        'tests/fixtures/language/run_stream_analysis.phys','tests/language_memory_balance.h',
        'tests/prepare_language_stream_corruption.py','tests/test_language_run_writer.c',
        'tests/fixtures/language/run_writer.phys','tests/check_language_writer_parity.py',
        'tests/test_mean_interval.c','tests/mean_interval_probe.c','tests/check_mean_interval_report.c',
        'tests/test_mean_interval_oracle.py','tests/fixtures/language/mean_interval.phys',
        'tests/fixtures/language/mean_interval_oracle.phys','tests/fixtures/language/analysis_mean_interval.phys',
        'tests/test_positive_distributions.c','tests/positive_distribution_probe.c','tests/test_positive_distribution_oracle.py',
        'tests/fixtures/language/positive_distributions.phys','tests/fixtures/language/positive_distribution_oracle.phys',
        'tests/test_run_events.c','tests/run_event_probe.c','tests/run_event_legacy_probe.c','tests/test_run_event_codec.py',
        'tests/fixtures/language/run_events.phys','tests/fixtures/run_event_failure.c','tests/test_run_event_failure.py',
        'tests/test_runner_identity.py','tests/fixtures/run_identity_collision.c','tests/test_project_identity_workflow.py',
        'tests/test_typed_run_identity.c','tests/typed_identity_probe.c','tests/test_typed_run_identity_codec.py',
        'tests/fixtures/language/typed_run_identity.phys','tests/test_build_origin.py',
        'tests/fixtures/build_origin_legacy.c','tests/fixtures/build_origin_legacy.h','tests/fixtures/build_origin_invalid.c',
        'tests/test_run_start_time.c','tests/start_time_probe.c','tests/test_run_start_time_codec.py',
        'tests/fixtures/language/run_start_time.phys','tests/run_archive_reference.py'}<=set(names)
assert not any(name.startswith(('src/','include/','lib/')) for name in names)
assert all(not name.startswith('app/') or name=='app/utf8.manifest' for name in names)
with tarfile.open(archive) as file:
    assert set(file.getnames())==set(names)|{'verification-kit.json'}
    manifest=json.load(file.extractfile('verification-kit.json'))
    assert manifest['format']==1 and set(manifest['files'])==set(names)
    for name,digest in manifest['files'].items():
        assert hashlib.sha256(file.extractfile(name).read()).hexdigest()==digest
        assert (repo/name).read_bytes()==file.extractfile(name).read()
original=archive.read_bytes()
try:kit.package(repo,archive);raise AssertionError('Existing kit was replaced')
except FileExistsError:pass
assert archive.read_bytes()==original
fixture=root/'fixture';fixture.mkdir()
for name in names:
    target=fixture/name;target.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(repo/name,target)
missing=fixture/'tools/sdk_series_probe.c';missing.unlink()
try:kit.package(fixture,root/'incomplete.tar.gz');raise AssertionError('Missing probe was packaged')
except ValueError as error:assert 'sdk_series_probe.c' in str(error)
assert not (root/'incomplete.tar.gz').exists()
missing.write_bytes((repo/'tools/sdk_series_probe.c').read_bytes())
verifier=fixture/'tools/verify-native-sdk.py';source=verifier.read_text()
for extra in ('repo / path','repo / "src/core.c"','repo / "../private.c"','repo / "app/main.c"'):
    verifier.write_text(source+'\n'+extra+'\n')
    try:kit.inputs(fixture);raise AssertionError(f'Invalid input accepted: {extra}')
    except ValueError:pass
verifier.write_text(source)
# An interrupted write removes its own unpublished output only.
from unittest.mock import patch
with patch.object(kit.tarfile,'open',side_effect=OSError('injected archive failure')):
    try:kit.package(fixture,root/'failed.tar.gz');raise AssertionError('Failed archive was accepted')
    except OSError:pass
assert not (root/'failed.tar.gz').exists() and archive.read_bytes()==original
print(f'Verification kit: {len(names)} exact inputs, independent sources, SHA-256, missing/dynamic paths and exclusive publication passed')
