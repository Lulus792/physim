"""Independent PNG/zlib decoder checks. Usage: python verify_png.py BUILD [PLOT_TEST_DIR].

Uses only the Python standard library; Pillow is used separately for visual review.
"""
from pathlib import Path
import struct
import sys
import zlib


def decode(path):
    data = path.read_bytes()
    assert data[:8] == b"\x89PNG\r\n\x1a\n", path
    pos, parts, kinds = 8, [], []
    while pos < len(data):
        length = struct.unpack_from(">I", data, pos)[0]
        kind = data[pos + 4:pos + 8]
        payload = data[pos + 8:pos + 8 + length]
        crc = struct.unpack_from(">I", data, pos + 8 + length)[0]
        assert zlib.crc32(kind + payload) == crc, (path, kind)
        kinds.append(kind)
        if kind == b"IHDR":
            width, height, depth, color, compression, filtering, interlace = struct.unpack(">IIBBBBB", payload)
            assert (depth, color, compression, filtering, interlace) == (8, 2, 0, 0, 0)
        elif kind == b"IDAT":
            parts.append(payload)
        elif kind == b"IEND":
            assert not payload
        else:
            raise AssertionError(kind)
        pos += length + 12
    assert pos == len(data) and kinds == [b"IHDR"] + [b"IDAT"] * len(parts) + [b"IEND"]
    decoder = zlib.decompressobj()
    raw = decoder.decompress(b"".join(parts)) + decoder.flush()
    assert decoder.eof and not decoder.unused_data and not decoder.unconsumed_tail
    stride = width * 3 + 1
    assert len(raw) == height * stride
    assert all(raw[y * stride] == 0 for y in range(height))
    pixels = b"".join(raw[y * stride + 1:(y + 1) * stride] for y in range(height))
    return width, height, pixels


def pixel(pixels, x, y, width=2400):
    i = (y * width + x) * 3
    return pixels[i:i + 3]


build = Path(sys.argv[1])
w, h, pixels = decode(build / "png-reference.png")
assert (w, h) == (64, 65)
assert pixels == bytes(v for y in range(65) for x in range(64) for v in (x * 3, y * 3, x ^ y))
for i, size in enumerate(((1, 1), (8192, 1), (1, 8192))):
    w, h, pixels = decode(build / f"png-reference.png.edge-{i}.png")
    assert (w, h) == size and pixels == bytes((7, 8, 9)) * (w * h)
w, h, pixels = decode(build / "png-reference.png.noise.png")
state, expected = 12345, bytearray()
for _ in range(257 * 131 * 3):
    state = (state * 1664525 + 1013904223) & 0xffffffff
    expected.append(state >> 24)
assert (w, h) == (257, 131) and pixels == expected
assert (build / "png-reference.png.edge-2.png").stat().st_size < 1000
print("PNG references: exact pixels, padded rows, limits, noise and compression passed")
if len(sys.argv) == 2:
    sys.exit(0)
plots = Path(sys.argv[2])
palette = [(23, 109, 209), (188, 75, 18), (23, 132, 80), (141, 74, 184),
           (162, 37, 86), (20, 127, 139), (117, 99, 19), (83, 97, 115)]
for name in ("0", "1", "2", "3", "dense"):
    path = plots / f"figure-{name}.png"
    w, h, pixels = decode(path)
    assert (w, h) == (2400, 1700)
    assert pixel(pixels, 20, 20) == b"\xff\xff\xff"
    assert pixel(pixels, 2300, 1670) == b"\xff\xff\xff"
    assert pixels.count(bytes(palette[0])) > 100
    if path.name == "figure-2.png":
        # Independent expected locations for five bars with counts 1,2,3,4,5.
        for i in range(5):
            x, top = 470 + i * 408, 1160 - (i + 1) * 172
            assert pixel(pixels, x, top + 30) == bytes(palette[0]), i
            assert pixel(pixels, x, top - 20) == b"\xff\xff\xff", i
    if path.name == "figure-dense.png":
        for i, color in enumerate(palette):
            assert pixels.count(bytes(color)) > 1000, i
            # All eight legend markers, including the final row, must survive.
            x, y = 144 + (i % 2) * 1100, 1392 + (i // 2) * 60
            assert pixel(pixels, x, y) == bytes(color), i
        assert any(v < 180 for v in pixels[1628 * w * 3:1672 * w * 3])
    print(f"{path.name}: CRC, zlib, dimensions, orientation and expected pixels passed")
print("PNG references passed")
for i in range(3):
    w, h, pixels = decode(plots / f"figure-region-{i}.png")
    assert (w, h) == (1200, 850)
    ink = bytes((23, 109, 209))
    if i == 0:
        # Both line endpoints are outside, but the crossing segment is visible.
        for x in (120, 500, 1135):
            assert pixel(pixels, x, 365, w) == ink
    elif i == 1:
        # A bar extends beyond all four bounds; its visible interior is filled.
        for x, y in ((120, 152), (600, 350), (1134, 577)):
            assert pixel(pixels, x, y, w) == ink
    else:
        assert all(pixel(pixels, x, 365, w) != ink for x in range(118, 1139))
    assert pixel(pixels, 1160, 365, w) == b"\xff\xff\xff"
print("PNG region: crossing lines, clipped bars and excluded scatter points passed")
