"""Exact recovery, repeated capture versions, exclusions and interrupted publication."""
import argparse
import hashlib
import importlib.util
import json
from pathlib import Path
from unittest.mock import patch

parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--work',type=Path,required=True)
a=parser.parse_args();a.work.mkdir(parents=True,exist_ok=True)
repo=Path(__file__).resolve().parent.parent
spec=importlib.util.spec_from_file_location('bitmap_archive',repo/'tools/archive_bitmaps.py')
module=importlib.util.module_from_spec(spec);spec.loader.exec_module(module)
# Exercise the same CP1252 default as a captured Windows Python process.
original_read_text = Path.read_text
def legacy_read_text(path, *args, **kwargs):
    if not args and kwargs.get('encoding') is None: kwargs['encoding'] = 'cp1252'
    return original_read_text(path, *args, **kwargs)
legacy_encoding = patch.object(Path, 'read_text', legacy_read_text)
legacy_encoding.start()
root=a.work/'Generated evidence ä';root.mkdir()
original=root/'view β.bmp';data=b'BM'+bytes(range(256))*4000
original.write_bytes(data)
protected=root/'SDK';protected.mkdir();(protected/'asset.bmp').write_bytes(data)
external=a.work/'external.bmp';external.write_bytes(data)
(root/'foreign.bmp').symlink_to(external.resolve())
assert module.archive_bitmaps(root,[protected],phase=1)[0]==1
assert not original.exists() and external.read_bytes()==data and (protected/'asset.bmp').read_bytes()==data
assert module.restore_bitmaps(root)==1 and original.read_bytes()==data
assert module.restore_bitmaps(root)==0
# New captures retain both compressed versions; recovery selects the latest.
new=data+b'latest';original.write_bytes(new)
assert module.archive_bitmaps(root,[protected],phase=2)[0]==1
state=json.loads((root/'bitmap-archive.json').read_text(encoding='utf-8'));assert len(state['files'])==2
assert len(list(root.glob('view β.bmp.*.gz')))==2
assert module.restore_bitmaps(root)==1 and original.read_bytes()==new
original.write_bytes(b'BM foreign owner')
try:module.restore_bitmaps(root);raise AssertionError('Restore replaced foreign content')
except FileExistsError:pass
assert original.read_bytes()==b'BM foreign owner'
original.unlink()
# Invalid archives cannot be restored and never publish a partial BMP.
latest=root/state['files'][-1]['archive'];saved=latest.read_bytes();latest.write_bytes(b'corrupt')
try:module.restore_bitmaps(root);raise AssertionError('Corrupt archive accepted')
except ValueError:pass
assert not original.exists();latest.write_bytes(saved)
assert module.restore_bitmaps(root)==1 and original.read_bytes()==new
# Receipt publication failure preserves the original even if compression succeeded.
with patch.object(module,'publish_receipt',side_effect=OSError('injected receipt failure')):
 try:module.archive_bitmaps(root,[protected],phase=3);raise AssertionError('Failure hidden')
 except OSError:pass
assert original.read_bytes()==new
assert module.archive_bitmaps(root,[protected],phase=3)[0]==1
# Failed compression removes only its own temporary, leaving image/receipt intact.
original.write_bytes(data+b'next')
receipt=(root/'bitmap-archive.json').read_bytes()
with patch.object(module.gzip.GzipFile,'write',side_effect=OSError('injected compression failure')):
 try:module.archive_bitmaps(root,[protected]);raise AssertionError('Failure hidden')
 except OSError:pass
assert original.read_bytes()==data+b'next' and (root/'bitmap-archive.json').read_bytes()==receipt
assert not list(root.glob('*.pending'))
# Existing temporary files are someone else's and stay untouched.
temporary=original.with_name(original.name+'.'+module.digest(original)+'.gz.pending');temporary.write_bytes(b'foreign')
try:module.archive_bitmaps(root,[protected]);raise AssertionError('Temporary replaced')
except FileExistsError:pass
assert temporary.read_bytes()==b'foreign' and original.read_bytes()==data+b'next'
# Untrusted receipt paths cannot reach a file outside the proof root.
invalid = dict(state['files'][-1]); invalid['original'] = '../external.bmp'
(root/'bitmap-archive.json').write_text(json.dumps({'format':1,'files':[invalid]}),encoding='utf-8')
try:module.restore_bitmaps(root);raise AssertionError('Escaping receipt accepted')
except ValueError:pass
assert external.read_bytes()==data
print('Bitmap archive: exact bytes, latest capture recovery, protected SDK/assets, symlinks, corrupted archives and publication failures passed')

legacy_encoding.stop()
