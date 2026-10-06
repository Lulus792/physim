"""Independent format-1 reader and index decoder, without Physim code."""
from pathlib import Path
import struct
import sys
import zlib

def inspect(path):
    rows = scenes = pages = 0
    expected = []
    stored = []
    root = None
    start = None
    complete = False
    recovered = False
    with path.open('rb') as file:
        assert file.read(16) == b'PSRUN17\n' + struct.pack('<II', 1, 0x01020304)
        while header := file.read(12):
            offset = file.tell()-12
            assert len(header) == 12
            kind, size, crc = struct.unpack('<III', header)
            assert size <= 8192
            data = file.read(size)
            if len(data) != size or zlib.crc32(data) != crc:
                recovered = True
                break
            if kind == 3:
                time, x, velocity = struct.unpack('<ddd', data)
                assert x == rows and velocity == -rows
                assert time == (-3 if rows == 777 and path.name != 'million.psrun' else rows*.001)
                if rows % 256 == 0:
                    expected.append((3, 0, rows, offset, time))
                rows += 1
            elif kind == 5:
                assert struct.unpack_from('<I', data)[0] == 3
                time = struct.unpack_from('<d', data, 4)[0]
                expected.append((5, 0, scenes, offset, time))
                scenes += 1
            elif kind == 6:
                if start is None:
                    start = offset
                version, stride, count, reserved = struct.unpack_from('<4I', data)
                assert (version, stride, reserved) == (1, 256, 0)
                assert 0 < count <= 255 and size == 16+32*count
                stored.extend(struct.unpack_from('<IIQQd', data, 16+32*i) for i in range(count))
                pages += 1
            elif kind == 7:
                root = struct.unpack('<IIQQQQ', data)
            elif kind == 4:
                assert size == 8 and struct.unpack('<Q', data)[0] == rows
                complete = True
                assert not file.read(1)
                break
            # Frozen pre-index behavior: every other valid CRC chunk is skipped.
    return rows, scenes, complete, recovered, expected, stored, root, pages, start

directory = Path(sys.argv[1])
for name in ('index α.psrun', 'million.psrun'):
    rows, scenes, complete, recovered, expected, stored, root, pages, start = inspect(directory/name)
    assert complete and not recovered and expected == stored
    assert root == (1, 256, start, pages, rows, scenes)
    assert rows == (1000000 if name == 'million.psrun' else 1000)
for mode in range(1, 6):
    rows, scenes, complete, recovered, expected, stored, root, pages, start = inspect(directory/f'variant-{mode}.psrun')
    assert (rows, scenes) == (1000, 334)
    assert complete == (mode not in (2, 4)) and recovered == (mode == 4)
    if mode in (1, 2):
        assert not stored and root is None
    elif mode == 3:
        assert stored != expected
    elif mode == 5:
        assert stored == expected and root[0] == 2
print('Independent codec: million-row offsets, paged CRC index, root/footer, legacy skipping and corrupt/recovered variants passed')
