#ifndef PHYSIM_TEXT_DOCUMENT_H
#define PHYSIM_TEXT_DOCUMENT_H
#include "autosave.h"
#include "physim/core.h"
typedef enum {
    PS_DOCUMENT_OK,
    PS_DOCUMENT_IO,
    PS_DOCUMENT_INVALID,
    PS_DOCUMENT_LIMIT,
    PS_DOCUMENT_MEMORY,
    PS_DOCUMENT_CONFLICT
} ps_document_result;
typedef struct {
    char path[4096];
    char *saved;
    size_t length;
} ps_text_document;
/* Initialize to zero. path is the resolved absolute file path (symlink target).
 * Open/reload replace the saved snapshot only on success.
 * UTF-8 text is limited to 256 KiB; binary data is rejected, never truncated. */
ps_document_result ps_text_document_open(ps_text_document *document, const char *path);
void ps_text_document_destroy(ps_text_document *document);
/* Compares the current file with the saved snapshot before replacing it. Writes
 * unique sibling temporary files and a .bak copy of the previous source. A
 * failed operation preserves the editor snapshot and the source file. Concurrent
 * writers between the last comparison and rename are not locked out. */
ps_document_result ps_text_document_save(ps_text_document *document, const char *text,
                                         size_t length);
/* Compares filesystem identity, including symlinks/hardlinks and Windows path
 * spelling. Returns false if either path cannot be opened. */
bool ps_text_document_same_file(const char *left, const char *right);
/* Per-file recovery in an app-owned directory, outside the source tree. Uses a
 * document marker and the canonical source path; never writes the source.
 * Corrupt or foreign bundles must be preserved until explicitly discarded.
 * Concurrent writers to the same source are unsupported, as for project autosave. */
ps_result ps_document_draft_path(const char *directory, const char *source, char path[4096]);
ps_result ps_document_draft_read(const char *directory, const char *source, ps_autosave **out);
ps_result ps_document_draft_write(const char *directory, const ps_text_document *document,
                                  const char *text, size_t length, uint64_t timestamp);
ps_result ps_document_draft_discard(const char *directory, const char *source);
#endif
