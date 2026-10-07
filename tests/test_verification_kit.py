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
assert {'tools/sdk_series_probe.c','tests/test_saved_run_tutorial.py','tests/test_allocator.h'}<=set(names)
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
