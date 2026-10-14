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

Local patch (2026-10-15): `nk_font_config.ttf_index` selects a collection face
through baking. The default remains zero. Invalid negative/out-of-range indices
fail the bounded bake. Physim's face loader matches non-italic OpenType `head`
bold metadata, and fallback source lookup uses the selected index too.
`font_faces` generates a TTC from tracked fonts, checks index 2 against the
standalone bold raster, rejects an italic entry, and checks ownership failures.

Local patch (2026-10-15): `nk_font_atlas_clear` also releases pixel memory when
an atlas is discarded after a successful bake and before `nk_font_atlas_end`,
including a failed GPU upload. `font_atlas_limits` abandons both Alpha8 and
RGBA32 output before atlas end and checks complete allocator balance.
`font_bank_ui` exercises the real GL error path and retained older textures.

Local patch (2026-10-14): merged fonts extend the last handle and return that
handle. Each baked glyph records whether its source cmap contains the codepoint;
lookup skips `.notdef` entries in overlapping ranges. This remains valid after
source-blob cleanup. Adds publish lists only after all allocations succeed,
and owned fallback ranges survive blob cleanup until atlas clear.
`font_fallback` reproduces the original wrong-handle and overlapping-source
failures. `font_sources` covers missing source glyphs, actual raster ink,
allocation failures, retry and complete release at 16/32 pixels.

Local patch (2026-10-14): `nk_edit_string_scrolled` accepts an optional persistent
horizontal offset for an unfocused string field. Its caller owns that offset;
the widget preserves the active field's focus, selection, cursor and value.
The existing `nk_edit_string` entry point passes no offset and keeps its prior
behavior. `editor_scroll` inspects actual text commands across multiple frames
while another field retains focus and selection. F1 uses the new entry point
for queued UIA text-range reveal requests.

Local patch (2026-10-14): `nk_font_atlas_bake_limited` bounds both texture axes
by the renderer's `GL_MAX_TEXTURE_SIZE`. The packer widens and retries when the
complete glyph set does not fit; every glyph and the custom cursor rectangle
must be packed. A partial atlas is rejected. Packing allocations are released
on retries and pixel allocation failure. The original bake entry point retains
its 32768 dimension ceiling and now also requires complete packing. The embedded
`font_atlas_limits` regression covers width growth, glyph pixels, small and
non-power-of-two limits, allocation failures, cleanup and retry.

Local patch (2026-10-11): UTF-8 range deletion resolves both rune endpoints
against the original byte buffer instead of shifting its pointer while
retaining the original byte length. Suffix removal uses the exact byte
span, and clearing all runes also clears the byte count. The standalone
`utf8_string_bounds` test checks every deletion range in an exactly sized,
unterminated multi-byte buffer, suffix removal and invalid ranges. An
AddressSanitizer counterexample reproduces the former end-buffer read.

The local text command uses a C17 flexible array for its dynamically allocated
text tail. UBSan exposed fixed `char[2]` indexing beyond the declared member
during actual rendered UTF-8 labels; the existing allocation already reserves
all text bytes and the terminating NUL. The change corrects the declaration
without weakening bounds instrumentation.

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
Local patch (2026-10-11): `NK_EDIT_PRESERVE_UNDO` retains the undo/redo state
when a caller-owned persistent `nk_edit_buffer` is reactivated. Physim enables
this only for its code editors; transient `nk_edit_string` fields retain the
upstream reset behavior. Editor initialization, source reload and existing
history size limits are unchanged. `editor_replace_workflow` checks actual
search/replace, focus return, single-step undo/redo and empty replacement in
experiment, analysis and additional-document editors.
Local patch (2026-10-11): readonly edit widgets retain mouse selection,
copy, navigation and scrolling. A filtered input copy rejects text, deletion,
paste/cut, undo/redo and edit-mode changes; disabled widgets and ROM windows
remain inert. Consumed mouse scrolling is propagated to the parent input;
keyboard scroll commands stay in the active field. Cursor
jumps reveal the target row directly instead of scrolling just one row.
`report_details_workflow` checks actual readonly focus, attempted changes,
end scrolling, full clipboard contents and preserved dirty project editors.

Local patch (2026-09-28): a singleton off-curve TrueType contour wraps to its
own point instead of reading the next contour or beyond the temporary vertex
array. AddressSanitizer found this during system-font baking; the synthetic
`font_shape` test covers one contour and adjacent singleton contours without
requiring an installed font. The affected routine is also present in the
[upstream stb_truetype source](https://github.com/nothings/stb/blob/master/stb_truetype.h).
Local patch (2026-09-28): undo/redo collapses the current selection to the restored
cursor. A selection into removed text previously reached the renderer with invalid
bounds. `editor_clipboard` and the SDL editor regression cover this case.
Patched header SHA-256 (LF): 885260fb4e1d93d3d8a5c3e08e969ece37ab12de6b645a10e581cdca974d525c.

`nuklear_sdl3_renderer.h` is retained as the upstream `demo/sdl3_renderer` reference;
it is no longer compiled into Physim. Input and clipboard handling in `app/ui_sdl.c`
are adapted from that MIT-licensed backend. Physim's own `app/graphics.c` implements
OpenGL 3.3 rendering for both Nuklear and the 3D scene.

## Microsoft runtime (Windows package)

CMake's `InstallRequiredSystemLibraries` includes the redistributable Visual C++
runtime from the installed Visual Studio toolchain. These binaries retain their
Microsoft license; the Physim MIT license does not apply to them.

## Linux system D-Bus

The Linux app links dynamically against the system `libdbus-1.so.3` for its
AT-SPI provider. Development builds use the distribution’s `dbus-1` pkg-config
metadata; Linux packages declare `libdbus-1-3` as a runtime dependency.
The library is not vendored, copied into the SDK, or linked into the core.

## Offline fonts

Six unmodified Noto fallback fonts and their SIL OFL 1.1 notices are tracked in
`fonts/`. See `fonts/README.md` and `fonts/manifest.json` for pinned source and
byte hashes. They are distributed with the app and SDK; no system font or network
download is required for these fallback sources.

Local patch (2026-10-14): recoverable temporary, glyph, pixel and RGBA bake
allocation failures return through the cleanup path in debug builds as well;
assert dialogs no longer prevent a caller from retaining its prior font atlas.
The real F1 density workflow injects a failing candidate and continues with
the live atlas. Invalid API precondition assertions remain separate.
