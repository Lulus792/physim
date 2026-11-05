# ADR 026 — native C syntax foundation for completion

Date: 2026-11-05. Syntax foundation, lexical scope collector and subsequent C
completion-model/GUI integration are implemented and tested on Windows/Linux.
New macOS acceptance and separate signature help remain open.
Selected completion design D / ADR 023 is unchanged.

## Requirement and choice

C completion must use real C syntax for written variables, declarators and
parameters. The existing outline and browser demonstration are insufficient.
Use pinned Tree-sitter runtime 0.27.1 / C grammar 0.24.2, confined to the editor
adapter. Official release sources, original blob identities and license bytes
are recorded in `third_party/c-editor-grammar.json`. Both components are MIT;
the runtime's bundled ICU headers retain their separate Unicode license.
No service, provider worker, compiler subprocess, CLI or build-time download.

Alternatives: expanding the outline/browser lexer would repeat incomplete C
declaration heuristics. A complete compiler frontend would add toolchain and
preprocessing requirements to this lexical editor feature. Neither is a substitute
for the required visible-source scopes; no semantic/compiler acceptance claim.

## Ownership and limits

The upstream default allocator aborts on OOM. `app/c_syntax.c` compiles the unchanged
runtime through its existing `ts_malloc`/`calloc`/`realloc`/`free` hooks. Each call
has an explicit `ps_allocator`, tracked allocations and thread-local current owner.
Allocation failure or budget exhaustion leaves no parser object alive and returns
`PS_MEMORY`/`PS_LIMIT`. The C-only `setjmp` boundary never returns NULL into the
runtime, changes its global allocator or crosses application callbacks. Mutable
escape state is heap-owned. All library objects die before the call returns.

Default limits: 256 KiB original source, 32 MiB private heap including tracking,
10,000 parser progress checks and caller-owned node capacity. Progress checks are
an operation budget, not a measured wall-clock deadline. Output is a flat preorder
tree with original byte spans, parent indices and immutable grammar names. Errors
return zero count; callers discard partial output. No TS object pointer escapes.

## Incomplete-source finding and remaining implementation

Actual sanitizer probe: `void step(double dt){ double velocity=1; f(` gives an
ERROR subtree containing the function declarator, parameter and declaration, not
a normal function/compound-statement hierarchy. The complete counterpart parses
normally. A visitor that only accepts `function_definition` would silently lose
parameters/locals during typing. `app/c_bindings.c` now combines actual grammar-leaf
delimiter intervals and declarator edges to retain these scopes and prove
closed/foreign/future/unfinished-initializer exclusions. Complete recovered comma
declarators retain original specifier spans. No synthetic variable, source rewriting
or hand-written browser scope model. [Scope implementation and actual proof](../verification/c-bindings-2026-11-05.md).

Written C declarator forms, for-loop lifetime and outer/inner declaration order
are provided by the collector. Owner/revision binding, completion-model merge,
actual call/empty-argument context and native insertion/Undo are now integrated:
[actual scope, commands and Windows/Linux evidence](../verification/completion-c-2026-11-05.md).
Separate signature/parameter help still needs implementation and native acceptance.
Do not publish preprocessor/include
resolution, inferred types or runtime values from this syntax tree.

## Verification and provenance

Strict direct C17 GCC ASan/UBSan exercised real syntax, every 94 allocation
failure with cleanup/retry, memory/capacity/progress limits, independent parallel
domains and 1,024 malformed inputs. Registered Windows Release and Linux Debug
ASan/UBSan each passed 1/1. [Actual commands, provenance, warnings and limits](../verification/c-syntax-foundation-2026-11-05.md).
This does not establish macOS or GUI acceptance of the new backend.

The subsequent linked-app proof is separate from the original syntax-only test:
Windows/Linux each8/8 units and6/6 GUI groups, installed three-license bytes and
private app symbol audits. The GUI-independent core remains independent. No new
private-Mac access; current hosted macOS follow-up is still required.

Primary sources inspected 2026-11-05: [runtime release](https://github.com/tree-sitter/tree-sitter/releases/tag/v0.27.1),
[grammar release](https://github.com/tree-sitter/tree-sitter-c/releases/tag/v0.24.2),
the pinned C API/allocator implementations and both upstream advisory pages.
The advisory pages listed no publications in the displayed view; this is not a
general security clearance. Original source bytes and local negative/positive
build evidence remain available under `build/validation/c-scopes-2026-11-05`.
