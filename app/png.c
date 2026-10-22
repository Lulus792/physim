#include "png.h"
#include "physim/data.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
static void big32(unsigned char *p, uint32_t v) {
    p[0] = (unsigned char)(v >> 24);
    p[1] = (unsigned char)(v >> 16);
    p[2] = (unsigned char)(v >> 8);
    p[3] = (unsigned char)v;
}
/* Buffer includes four bytes before the payload for the chunk type. */
typedef struct {FILE *file;unsigned char *data;size_t size,capacity;bool memory;} png_sink;
static bool put(png_sink *sink,const void *data,size_t size) {
    if(!sink->memory)return fwrite(data,1,size,sink->file)==size;
    if(size>64u*1024u*1024u-sink->size)return false;
    size_t need=sink->size+size;
    if(need>sink->capacity) {
        size_t capacity=sink->capacity?sink->capacity:65536;
        while(capacity<need)capacity*=2;
        unsigned char *next=realloc(sink->data,capacity);if(!next)return false;
        sink->data=next;sink->capacity=capacity;
    }
    memcpy(sink->data+sink->size,data,size);sink->size=need;return true;
}
static bool chunk(png_sink *f, unsigned char *buffer, const char *type, size_t size) {
    unsigned char length[4], crc[4];
    memcpy(buffer, type, 4);
    big32(length, (uint32_t)size);
    big32(crc, ps_crc32(buffer, size + 4));
    return put(f,length,4) && put(f,buffer,size+4) && put(f,crc,4);
}
static ps_result encode(png_sink *f,const unsigned char *rgb,unsigned width,unsigned height,
    size_t stride,int compression) {
    if (!rgb || !width || !height || width > 8192 || height > 8192 ||
        stride < (size_t)width * 3 || stride > SIZE_MAX / height)
        return PS_INVALID;
    size_t row = (size_t)width * 3;
    enum { OUTPUT_SIZE = 65536 };
    unsigned char *input = malloc(row + 1);
    unsigned char *buffer = malloc(OUTPUT_SIZE + 4);
    if (!input || !buffer) {
        free(input);
        free(buffer);
        return PS_MEMORY;
    }
    z_stream stream = {0};
    int status = deflateInit(&stream, compression);
    if (status != Z_OK) {
        free(input);
        free(buffer);
        return status == Z_MEM_ERROR ? PS_MEMORY : PS_IO;
    }
    static const unsigned char signature[] = {137, 80, 78, 71, 13, 10, 26, 10};
    unsigned char header[17] = {0};
    big32(header + 4, width);
    big32(header + 8, height);
    header[12] = 8;
    header[13] = 2; /* RGB, no alpha; filter 0, non-interlaced. */
    bool ok = put(f,signature,8) && chunk(f, header, "IHDR", 13);
    for (unsigned y = 0; ok && y < height; y++) {
        input[0] = 0; /* PNG filter None */
        memcpy(input + 1, rgb + (size_t)y * stride, row);
        stream.next_in = input;
        stream.avail_in = (uInt)(row + 1);
        while (ok && stream.avail_in) {
            stream.next_out = buffer + 4;
            stream.avail_out = OUTPUT_SIZE;
            status = deflate(&stream, Z_NO_FLUSH);
            size_t produced = OUTPUT_SIZE - stream.avail_out;
            ok = status == Z_OK && (!produced || chunk(f, buffer, "IDAT", produced));
        }
    }
    while (ok) {
        stream.next_out = buffer + 4;
        stream.avail_out = OUTPUT_SIZE;
        status = deflate(&stream, Z_FINISH);
        size_t produced = OUTPUT_SIZE - stream.avail_out;
        ok = (status == Z_OK || status == Z_STREAM_END) &&
             (!produced || chunk(f, buffer, "IDAT", produced));
        if (status == Z_STREAM_END)
            break;
    }
    deflateEnd(&stream);
    if (ok)
        ok = chunk(f, buffer, "IEND", 0);
    free(buffer);
    free(input);
    return ok ? PS_OK : (status == Z_MEM_ERROR ? PS_MEMORY : PS_IO);
}
static ps_result write_png(const char *path,const unsigned char *rgb,unsigned width,unsigned height,
    size_t stride,int compression) {
    if(!path || !*path || !rgb || !width || !height || width>8192 || height>8192 ||
       stride<(size_t)width*3 || stride>SIZE_MAX/height)return PS_INVALID;
    png_sink sink={0};sink.file=fopen(path,"wbx");if(!sink.file)return PS_IO;
    ps_result result=encode(&sink,rgb,width,height,stride,compression);
    if(fclose(sink.file) && result==PS_OK)result=PS_IO;
    if(result!=PS_OK)remove(path);
    return result;
}
ps_result ps_png_encode(const unsigned char *rgb,unsigned width,unsigned height,size_t stride,
    unsigned char **out,size_t *size) {
    if(!out || !size)return PS_INVALID;
    png_sink sink={0};sink.memory=true;
    ps_result result=encode(&sink,rgb,width,height,stride,Z_BEST_SPEED);
    if(result!=PS_OK){free(sink.data);return result;}
    *out=sink.data;*size=sink.size;return PS_OK;
}
ps_result ps_png_write(const char *path,const unsigned char *rgb,unsigned width,unsigned height,size_t stride) {
    /* Compression speed changes file size, never image quality or RGB bytes. */
    return write_png(path,rgb,width,height,stride,Z_BEST_SPEED);
}
