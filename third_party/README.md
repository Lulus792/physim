# UI dependencies

No file in this directory is linked into the GUI-independent core library.

## SDL3 3.2.30

Official source: <https://github.com/libsdl-org/SDL/tree/release-3.2.30>

Windows development archive:
<https://github.com/libsdl-org/SDL/releases/download/release-3.2.30/SDL3-devel-3.2.30-VC.zip>

SHA-256: `3a93c2182ddaf64692a8907928e6a54467f0f7c1caddf3019cfa69fbf1dfe3f2`.
The binary archive and extracted directory are ignored by Git. Bootstrap with
`tools/bootstrap-windows.ps1`. License: zlib, distributed in the archive and package.

## zlib 1.3.2

Unmodified upstream source from <https://zlib.net/fossils/zlib-1.3.2.tar.gz>.
Archive SHA-256: `bb329a0a2cd0274d05519d61c667c062e06990d72e125ee2dfa8de64f0119d16`.
The compression sources are statically linked with `Z_PREFIX` for PNG export;
no codec DLL or build-time download is required. License: `zlib-1.3.2/LICENSE`,
included as `licenses/zlib-LICENSE.txt` in the portable package.

## Nuklear snapshot

Official source: <https://github.com/Immediate-Mode-UI/Nuklear>.
Vendored source snapshot retrieved 2026-09-16. Files are committed, never downloaded
implicitly at build time. License text: `Nuklear-LICENSE`; the MIT alternative is used.

- `nuklear.h` upstream snapshot: SHA-256 `30a33586aeee98f71db083a6c1546295a4279fd8f214cfab36eec703eb98ebfd`
- `nuklear_sdl3_renderer.h`: SHA-256 `cd625f7093379c60650002451ad1468083c474e4f076ad55a833e614fcb9904c`


Local patch (2026-09-19): `nk_textedit_paste` accepts UTF-8 byte lengths, validates
complete canonical encoding, advances cursor/undo by Unicode scalar count, and
reserves space before removing a selection. Selection replacement is one undo
record. Fixed-buffer exhaustion and dynamic allocation failure preserve text,
selection and undo. Paste input must not alias the editor buffer. Existing Nuklear
undo-history size limits still apply. `app/ui_sdl.c` passes bytes and limits a paste
to 262144 bytes. The copy callback retains upstream scalar-count semantics.
Regression target: `physim-editor-clipboard-tests`; SDL integration: `--renderer-test`.
Local patch (2026-09-28): the Tab handler uses the same transactional insertion
to insert four spaces as one undo record, including selection replacement.
Both project editors and additional documents use this handler. The
`documents_small` and `documents_large` SDL tests cover Tab, undo and redo.
Local patch (2026-09-28): a singleton off-curve TrueType contour wraps to its
own point instead of reading the next contour or beyond the temporary vertex
array. AddressSanitizer found this during system-font baking; the synthetic
`font_shape` test covers one contour and adjacent singleton contours without
requiring an installed font. The affected routine is also present in the
[upstream stb_truetype source](https://github.com/nothings/stb/blob/master/stb_truetype.h).
Patched header SHA-256: 95cbb20e766bed5a42bb7abb32e3bcf0a55eefebe3498a3f277ddde88f14a913.

`nuklear_sdl3_renderer.h` is retained as the upstream `demo/sdl3_renderer` reference;
it is no longer compiled into Physim. Input and clipboard handling in `app/ui_sdl.c`
are adapted from that MIT-licensed backend. Physim's own `app/graphics.c` implements
OpenGL 3.3 rendering for both Nuklear and the 3D scene.

## Microsoft runtime (Windows package)

CMake's `InstallRequiredSystemLibraries` includes the redistributable Visual C++
runtime from the installed Visual Studio toolchain. These binaries retain their
Microsoft license; the Physim MIT license does not apply to them.
