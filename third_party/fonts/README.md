# Offline fallback fonts

Unmodified TrueType binaries from the official Google Fonts repository at commit
`66928797f271fed5ee428544e7229685e2d90127`, retrieved 2026-10-14.
The shorter local filenames preserve the font bytes and internal family names.
The static bold and italic serif faces also come from the official archived Noto distribution,
revision `ffebf8c1ee449e544955a7e813c54f9b73848eac` of `notofonts/noto-fonts`.
No font download occurs during a build or at runtime.

| Local file | Pinned upstream file | License |
| --- | --- | --- |
| NotoSans.ttf | ofl/notosans/NotoSans[wdth,wght].ttf | notosans-OFL.txt |
| NotoSans-Bold.ttf | hinted/ttf/NotoSans/NotoSans-Bold.ttf (Noto distribution) | notosansbold-OFL.txt |
| NotoSerif-Italic.ttf | hinted/ttf/NotoSerif/NotoSerif-Italic.ttf (Noto distribution) | notoserifitalic-OFL.txt |
| NotoSansMono.ttf | ofl/notosansmono/NotoSansMono[wdth,wght].ttf | notosansmono-OFL.txt |
| NotoSansMono-SemiBold.ttf | hinted/ttf/NotoSansMono/NotoSansMono-SemiBold.ttf (Noto distribution) | notosansbold-OFL.txt (shared Noto license) |
| NotoSansMath-Regular.ttf | ofl/notosansmath/NotoSansMath-Regular.ttf | notosansmath-OFL.txt |
| NotoSansSymbols2-Regular.ttf | ofl/notosanssymbols2/NotoSansSymbols2-Regular.ttf | notosanssymbols2-OFL.txt |
| NotoEmoji.ttf | ofl/notoemoji/NotoEmoji[wght].ttf | notoemoji-OFL.txt |

Source: [Google Fonts snapshot](https://github.com/google/fonts/tree/66928797f271fed5ee428544e7229685e2d90127/ofl).
Static bold source: [pinned Noto distribution](https://github.com/notofonts/noto-fonts/tree/ffebf8c1ee449e544955a7e813c54f9b73848eac).
Each font retains its accompanying copyright and SIL Open Font License 1.1.
`manifest.json` records the exact source paths, byte lengths and SHA-256 hashes
for both binaries and licenses. Entries without a repository override use the
Google Fonts source; the bold and italic serif binaries/licenses specify the Noto repository and
revision explicitly. The installed SDK and macOS Resources include
this directory; the package manifest also hashes its files.

Physim uses the default outlines of variable fonts, without variation-axis
selection, and the actual static bold face for strong roles when needed.
Workspace C/P markers have a separate 12-CSS-pixel strong monospace role. It uses
Consolas Bold or the available platform mono bold face, with the pinned static
Noto Sans Mono SemiBold (weight 600) as its offline fallback. The unchanged Noto
license above applies to that additional unmodified face too. This bounded
ASCII role is independent of editor font-size preferences.
Emoji use monochrome outlines supported by the existing rasterizer.
Fallback ranges cover the application's Latin/Greek text, its phonetic modifiers,
mathematics, box drawing, U+25CF, U+2603, U+1F30D and U+1F600. This is a
bounded character repertoire, not a claim of universal Unicode or shaping support.
Only codepoints absent from earlier declared source ranges are merged, so the
platform font retains its existing metrics and glyphs. The current 185-page inventory
checks 183 distinct printable source/reading characters across all 42 F1 text handles; U+0378 remains
a negative missing-source case. Further scripts and shaping require their own
fonts, layout implementation and actual render checks.

Box drawing is declared in the primary code font as well, so diagrams use the
same advance as their spaces. If a platform code font is unavailable, Noto Sans
Mono becomes the primary code font rather than mixing proportional ASCII with
monospaced box glyphs. The page inventory detects new characters outside this
repertoire; its limits must be expanded with source, raster and image proof.

The diagnostic environment `PHYSIM_TEST_BUNDLED_FONTS=1` bypasses platform fonts
in the real app. The character workflow runs both modes and checks all 57 primary
text handles (15 main-window and 42 F1), including the actual code diagram advances.

The separate F1 SVG-label role uses installed Georgia Italic when available,
then the platform's italic serif fallback and finally `NotoSerif-Italic.ttf`.
The ASCII base repertoire plus U+03B8 are baked into each of its four UI-size variants,
preserving the existing atlas upload checks for every printable ASCII glyph.
These are additional bounded diagram roles, not general text handles: the current
inventory checks 42 complete F1 text variants and separately all three model
labels in all four serif variants. The bundled diagnostic forces this role to
Noto Serif Italic as well. Georgia remains a system resource and is not included
in the SDK. No font is downloaded by a build or running app.
