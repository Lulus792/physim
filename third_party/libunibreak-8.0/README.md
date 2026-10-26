# libunibreak line- and grapheme-breaking subset

Unmodified source files from the official `libunibreak_8_0` release,
commit `28a2756b864c343f438cd22537d49d394d4666a5`:
[upstream release](https://github.com/adah1972/libunibreak/releases/tag/libunibreak_8_0).
The line- and grapheme-break algorithms and their dependencies are compiled
into Physim. Word-breaking implementations are not included. No runtime
download occurs. Grapheme breaking supports emergency wrapping of long
original messages without separating combining marks, emoji or conjuncts.

`manifest.json` records upstream paths, Git blob IDs, byte lengths and SHA256.
`UPSTREAM-README.md`, `NEWS` and `LICENCE` preserve the original texts.
The Unicode data notice is included as `UNICODE-LICENSE.txt`; both licenses
are copied into installed SDKs. The upstream `LineBreakTest.txt` is stored as
a reproducible gzip solely to reduce repository size. Decompression preserves
the exact original bytes and their recorded upstream hash; the full fixture
is used by the native conformance test. `GraphemeBreakTest.txt.gz` uses the same
reproducible storage and upstream byte verification for the complete grapheme
conformance fixture. Source/property tables are unchanged.

The application adds its own `pre-wrap` handling, glyph measurement and UI
scrolling in `app/code_preview.c`. Those are Physim code, not upstream changes.
