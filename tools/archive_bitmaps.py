"""Losslessly archive generated BMP evidence; restore exact bytes without replacement."""
import argparse
import gzip
import hashlib
import json
import os
from pathlib import Path


def digest(path):
    result = hashlib.sha256()
    with path.open('rb') as stream:
        while chunk := stream.read(1024*1024): result.update(chunk)
    return result.hexdigest()


def publish_receipt(path, value):
    temporary = path.with_name(path.name+'.pending')
    created = False
    try:
        with temporary.open('x', encoding='utf-8') as stream:
            created = True
            json.dump(value, stream, ensure_ascii=False, indent=2); stream.write('\n')
            stream.flush(); os.fsync(stream.fileno())
        os.replace(temporary, path)
    finally:
        if created: temporary.unlink(missing_ok=True)


def archive_bitmaps(root, protected=(), phase=0):
    root = Path(root).resolve(); protected = [Path(p).resolve() for p in protected]
    receipt = root/'bitmap-archive.json'
    if receipt.is_symlink(): raise ValueError('Receipt must not be a symlink')
    state = json.loads(receipt.read_text()) if receipt.exists() else {'format':1, 'files':[]}
    if state.get('format') != 1 or not isinstance(state.get('files'), list):
        raise ValueError('Invalid bitmap archive receipt')
    count = saved = 0
    for original in sorted(root.rglob('*.bmp')):
        if original.is_symlink() or not original.is_file(): continue
        resolved = original.resolve()
        if root not in resolved.parents or any(p == resolved or p in resolved.parents for p in protected): continue
        with original.open('rb') as stream:
            if stream.read(2) != b'BM': raise ValueError('Expected generated BMP: '+str(original))
        size = original.stat().st_size; original_hash = digest(original)
        archived = original.with_name(original.name+'.'+original_hash+'.gz')
        temporary = archived.with_name(archived.name+'.pending')
        created = False
        try:
            if not archived.exists():
                with temporary.open('xb') as output:
                    created = True
                    with gzip.GzipFile(filename='', fileobj=output, mode='wb', mtime=0) as compressed, original.open('rb') as source:
                        while chunk := source.read(1024*1024): compressed.write(chunk)
                    output.flush(); os.fsync(output.fileno())
                os.link(temporary, archived); temporary.unlink(); created = False
            if archived.is_symlink(): raise ValueError('Archive path must be a regular file')
            restored_hash = hashlib.sha256(); restored_size = 0
            with gzip.open(archived, 'rb') as stream:
                while chunk := stream.read(1024*1024): restored_hash.update(chunk); restored_size += len(chunk)
            if restored_hash.hexdigest() != original_hash or restored_size != size:
                raise ValueError('Bitmap archive differs from original')
            record = dict(original=original.relative_to(root).as_posix(),
                          archive=archived.relative_to(root).as_posix(), size=size,
                          sha256=original_hash, archive_sha256=digest(archived), phase=phase)
            if record not in state['files']: state['files'].append(record)
            publish_receipt(receipt, state)
            if original.is_symlink() or digest(original) != original_hash:
                raise ValueError('Bitmap changed while archiving; original retained')
            original.unlink(); count += 1; saved += size-archived.stat().st_size
        finally:
            if created: temporary.unlink(missing_ok=True)
    return count, saved


def restore_bitmaps(root):
    root = Path(root).resolve(); receipt = root/'bitmap-archive.json'
    if receipt.is_symlink(): raise ValueError('Receipt must not be a symlink')
    state = json.loads(receipt.read_text())
    if state.get('format') != 1: raise ValueError('Invalid bitmap archive receipt')
    count = 0
    # A repeated capture may share a path; restore the last recorded version.
    records = {entry['original']:entry for entry in state['files']}
    for record in records.values():
        original = root/record['original']; archived = root/record['archive']
        for path in (original, archived):
            if path.is_symlink() or root not in path.resolve().parents:
                raise ValueError('Bitmap receipt escapes its root')
        if original.exists():
            if digest(original) != record['sha256']: raise FileExistsError(original)
            continue
        if digest(archived) != record['archive_sha256']: raise ValueError('Compressed bitmap hash mismatch')
        temporary = original.with_name(original.name+'.restore.pending'); created = False
        try:
            restored_hash = hashlib.sha256(); size = 0
            with temporary.open('xb') as output:
                created = True
                with gzip.open(archived, 'rb') as stream:
                    while chunk := stream.read(1024*1024): output.write(chunk); restored_hash.update(chunk); size += len(chunk)
                output.flush(); os.fsync(output.fileno())
            if restored_hash.hexdigest() != record['sha256'] or size != record['size']:
                raise ValueError('Restored bitmap hash mismatch')
            os.link(temporary, original); count += 1
        finally:
            if created: temporary.unlink(missing_ok=True)
    return count


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('root', type=Path)
    parser.add_argument('--exclude', action='append', type=Path, default=[])
    parser.add_argument('--restore', action='store_true')
    args = parser.parse_args()
    if args.restore: print('Restored', restore_bitmaps(args.root), 'exact bitmap files')
    else: print('Archived bitmap files and freed bytes:', archive_bitmaps(args.root, args.exclude))
