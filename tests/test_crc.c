#include "crc_vectors.h"
#include "physim/data.h"
#include "physim/report.h"
#include "platform.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "CRC line %d: %s\n", __LINE__, #x);                                    \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static uint32_t reference(const unsigned char *bytes, size_t count) {
    uint32_t crc = UINT32_MAX;
    for (size_t i = 0; i < count; i++) {
        crc ^= bytes[i];
        for (unsigned bit = 0; bit < 8; bit++)
            crc = (crc >> 1) ^ ((crc & 1) ? UINT32_C(0xedb88320) : 0);
    }
    return ~crc;
}
static volatile uint32_t consumed;
int main(int argc, char **argv) {
    if (argc == 3 && !strcmp(argv[1], "--roundtrip")) {
        ps_report *report = NULL;
        char path[128];
        snprintf(path, sizeof path, "crc-roundtrip-%.0f.psreport", ps_clock() * 1e9);
        CHECK(ps_report_load(argv[2], &report) == PS_OK);
        CHECK(ps_report_save(report, path) == PS_OK);
        ps_report_destroy(report);
        FILE *before = fopen(argv[2], "rb"), *after = fopen(path, "rb");
        CHECK(before && after);
        int a, b;
        do {
            a = fgetc(before);
            b = fgetc(after);
            CHECK(a == b);
        } while (a != EOF);
        CHECK(!ferror(before) && !ferror(after));
        CHECK(!fclose(before) && !fclose(after));
        CHECK(!remove(path));
        puts("Historical report roundtrip is byte-identical");
        return 0;
    }
    bool benchmark = argc == 2 && !strcmp(argv[1], "--benchmark");
    if (argc != 1 && !benchmark)
        return 2;
    CHECK(ps_crc32(NULL, 0) == 0);
    CHECK(ps_crc32((const unsigned char *)"123456789", 9) == UINT32_C(0xcbf43926));
    size_t capacity = 8u * 1024u * 1024u;
    unsigned char *data = malloc(capacity + 32);
    CHECK(data != NULL);
    for (size_t i = 0; i < capacity + 32; i++)
        data[i] = (unsigned char)((i * 31) ^ (i >> 8) ^ 0xa5);
    for (size_t i = 0; i < sizeof golden / sizeof *golden; i++)
        CHECK(ps_crc32(data, golden[i].size) == golden[i].crc);
    for (unsigned value = 0; value < 256; value++) {
        unsigned char byte = (unsigned char)value;
        CHECK(ps_crc32(&byte, 1) == reference(&byte, 1));
    }
    /* Exercise byte alignment and every prefix of typical chunk sizes. */
    for (unsigned offset = 0; offset < 32; offset++)
        for (size_t size = 0; size <= 512; size++)
            CHECK(ps_crc32(data + offset, size) == reference(data + offset, size));
    uint32_t state = 17;
    for (unsigned trial = 0; trial < 1000; trial++) {
        state = state * UINT32_C(1664525) + UINT32_C(1013904223);
        size_t length = state % 8193;
        unsigned offset = (state >> 24) & 31;
        CHECK(ps_crc32(data + offset, length) == reference(data + offset, length));
    }
    uint32_t original = ps_crc32(data, 136);
    for (size_t bit = 0; bit < 136 * 8; bit++) {
        data[bit / 8] ^= (unsigned char)(1u << (bit % 8));
        CHECK(ps_crc32(data, 136) != original);
        data[bit / 8] ^= (unsigned char)(1u << (bit % 8));
    }
    if (benchmark) {
        const size_t sizes[] = {56, 136, 8192, 1024 * 1024};
        puts("bytes,repeat,calls,seconds,mib_per_second");
        for (size_t i = 0; i < sizeof sizes / sizeof *sizes; i++) {
            size_t calls = (16u * 1024u * 1024u) / sizes[i];
            for (unsigned repeat = 0; repeat < 5; repeat++) {
                double start = ps_clock();
                for (size_t j = 0; j < calls; j++)
                    consumed ^= ps_crc32(data, sizes[i]);
                double elapsed = ps_clock() - start;
                CHECK(elapsed > 0);
                printf("%zu,%u,%zu,%.9f,%.3f\n", sizes[i], repeat, calls, elapsed,
                       (double)(calls * sizes[i]) / (1024 * 1024) / elapsed);
            }
        }
    } else
        puts("CRC: zlib vectors, alignments, prefixes, all single-byte values and bit faults "
             "passed");
    free(data);
    return fflush(stdout) ? 1 : 0;
}
