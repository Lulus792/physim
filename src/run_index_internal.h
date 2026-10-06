#ifndef PS_RUN_INDEX_INTERNAL_H
#define PS_RUN_INDEX_INTERNAL_H
#include "physim/data.h"
#include <limits.h>
#ifndef _WIN32
#include <sys/types.h>
#endif
static inline bool ps_run_seek(FILE *file,uint64_t offset) {
    if(offset>INT64_MAX)return false;
#ifdef _WIN32
    return _fseeki64(file,(int64_t)offset,SEEK_SET)==0;
#else
    off_t position=(off_t)offset;
    return position>=0 && (uint64_t)position==offset && fseeko(file,position,SEEK_SET)==0;
#endif
}
static inline bool ps_run_position(FILE *file,uint64_t *out) {
#ifdef _WIN32
    int64_t position=_ftelli64(file);
#else
    off_t position=ftello(file);
#endif
    if(position<0)return false;
    *out=(uint64_t)position;return true;
}
ps_result ps_run_chunk_read(FILE *file,uint32_t *type,unsigned char *payload,uint32_t *size);
ps_result ps_run_write_index(FILE *file,uint32_t channels,uint64_t samples);
#endif
