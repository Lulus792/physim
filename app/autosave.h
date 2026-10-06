#ifndef PHYSIM_AUTOSAVE_H
#define PHYSIM_AUTOSAVE_H
#include "physim/core.h"
#define PS_SOURCE_MAX_BYTES (256u * 1024u)
/* Private app format: two draft sources followed by their last saved baselines.
 * Owned when returned by read; write borrows all four buffers. No embedded NUL,
 * malformed UTF-8 or controls except tab/CR/LF. Empty sources are valid. */
typedef struct {
    char *text[4];
    uint32_t length[4];
    uint64_t saved_at_s;
} ps_autosave;
/* Internal shared persistence helper. POSIX files start at 0600; mode_source
 * copies ordinary rwx bits before publication. Does not copy ACLs/xattrs.
 * On success caller owns temporary; on failure no owned file remains. */
ps_result ps_private_temporary_write(const char *path, const void *bytes, size_t length,
                                     const char *mode_source, char temporary[4096]);
bool ps_source_text_valid(const char *text, size_t length);
/* Write a complete CRC-protected bundle to an exclusive sibling file, close, then replace path.
 * A failed write never replaces the previous snapshot. Caller owns the path;
 * concurrent writers to one project are unsupported. No power-loss guarantee. */
ps_result ps_autosave_write(const char *path, const ps_autosave *snapshot);
/* Strict version, bounds, UTF-8, CRC and EOF validation. PS_EOF means absent.
 * Failure leaves out unchanged. Free successful results with destroy. */
ps_result ps_autosave_read(const char *path, ps_autosave **out);
void ps_autosave_destroy(ps_autosave *snapshot);
#endif
