#include "physim/run_index.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned long long cases, opened, rows;
static void require(bool ok) {
    if (ok)
        return;
    fprintf(stderr,
            "Run mutation invariant failed at case %llu; input retained as run-mutation.psrun\n",
            cases);
    abort();
}
static void exercise(const unsigned char *data, size_t size) {
    FILE *f = fopen("run-mutation.psrun", "wb");
    require(f != NULL);
    require(fwrite(data, 1, size, f) == size);
    require(fclose(f) == 0);
    cases++;
    ps_run_index *index=NULL;
    ps_result indexed=ps_run_index_open("run-mutation.psrun",ps_allocator_default(),128,&index);
    if(indexed==PS_OK || indexed==PS_RECOVERED) {
        require(index!=NULL);
        ps_run_index_info info={.struct_size=sizeof info,.version=PS_RUN_INDEX_VERSION};
        require(ps_run_index_get_info(index,&info)==PS_OK && info.complete==(indexed==PS_OK));
        require(info.samples<=size/12 && info.checkpoints<=128);
        if(info.samples) {
            double time,values[PS_MAX_CHANNELS];require(ps_run_index_read(index,0,1,&time,values)==PS_OK && isfinite(time));
            for(unsigned i=0;i<info.channels;i++)require(isfinite(values[i]));
        }
        if(info.snapshots){ps_snapshot snapshot;require(ps_run_index_snapshot(index,0,&snapshot)==PS_OK);}
        ps_run_index_destroy(index);
    } else {
        require(index==NULL && (indexed==PS_CORRUPT || indexed==PS_VERSION || indexed==PS_LIMIT));
    }
    ps_run_reader reader;
    ps_result result = ps_run_open(&reader, "run-mutation.psrun");
    if (result != PS_OK) {
        require(result == PS_CORRUPT || result == PS_VERSION);
        require(reader.file == NULL);
        return;
    }
    opened++;
    require(reader.channels > 0 && reader.channels <= PS_MAX_CHANNELS);
    require(memchr(reader.metadata, 0, sizeof reader.metadata) != NULL);
    for (unsigned i = 0; i < reader.channels; i++) {
        require(memchr(reader.schema[i].name, 0, sizeof reader.schema[i].name) != NULL);
        require(memchr(reader.schema[i].unit, 0, sizeof reader.schema[i].unit) != NULL);
    }
    uint64_t count = 0;
    for (;;) {
        double time = -17, values[PS_MAX_CHANNELS], saved[PS_MAX_CHANNELS];
        for (unsigned i = 0; i < PS_MAX_CHANNELS; i++)
            values[i] = saved[i] = -29.0 - i;
        result = ps_run_next(&reader, &time, values);
        if (result != PS_OK) {
            require(result == PS_EOF || result == PS_RECOVERED || result == PS_CORRUPT);
            require(time == -17 && !memcmp(values, saved, sizeof values));
            require(reader.complete == (result == PS_EOF));
            require(reader.samples == count);
            break;
        }
        require(isfinite(time));
        for (unsigned i = 0; i < reader.channels; i++)
            require(isfinite(values[i]));
        for (unsigned i = reader.channels; i < PS_MAX_CHANNELS; i++)
            require(values[i] == saved[i]);
        count++;
        rows++;
        require(count <= size / 12 && reader.samples == count);
    }
    ps_run_reader_close(&reader);
    require(reader.file == NULL);
}
int main(int argc, char **argv) {
    unsigned char seed[8192], changed[8192];
    if (argc == 3 && !strcmp(argv[1], "--replay")) {
        FILE *f = fopen(argv[2], "rb");
        if (!f)
            return 2;
        size_t size = fread(seed, 1, sizeof seed, f);
        int tail = fgetc(f);
        fclose(f);
        if (tail != EOF)
            return 2;
        exercise(seed, size);
        puts("Run replay passed");
        return 0;
    }
    if (argc != 1)
        return 2;
    ps_context context = {0};
    context.dt_s = .01;
    context.seed = 42;
    require(ps_channel_add(&context, "position", PS_METRE, "Position") == 0);
    require(ps_channel_add(&context, "energy", PS_JOULE, "Energy") == 1);
    require(ps_channel_add(&context, "angle", PS_RADIAN, "Angle") == 2);
    remove("run-seed.psrun");
    ps_run_writer writer;
    require(ps_run_create(&writer, "run-seed.psrun", &context, "mutation-reference") == PS_OK);
    for (unsigned i = 0; i < 3; i++) {
        double v[] = {i * .25, 2.0 + i, -.5 * i};
        require(ps_run_append(&writer, i * .01, v) == PS_OK);
    }
    require(ps_run_close(&writer) == PS_OK);
    FILE *f = fopen("run-seed.psrun", "rb");
    require(f != NULL);
    size_t size = fread(seed, 1, sizeof seed, f);
    require(fgetc(f) == EOF);
    require(fclose(f) == 0);
    size_t starts[8], lengths[8], chunks = 0;
    for (size_t at = 16; at < size;) {
        require(chunks < 8 && size - at >= 12);
        size_t length = ps_get_u32(seed + at + 4);
        require(length <= size - at - 12);
        starts[chunks] = at;
        lengths[chunks++] = length;
        at += 12 + length;
    }
    require(chunks == 8 && ps_get_u32(seed+starts[5])==6 && ps_get_u32(seed+starts[6])==7 && ps_get_u32(seed+starts[7])==4);
    exercise(seed, size);
    /* CRC-correct nonfinite last channel must not expose earlier values/time. */
    memcpy(changed, seed, size);
    ps_put_f64(changed + starts[2] + 12 + 24, NAN);
    ps_put_u32(changed + starts[2] + 8, ps_crc32(changed + starts[2] + 12, lengths[2]));
    exercise(changed, size);
    for (size_t cut = 0; cut < size; cut++)
        exercise(seed, cut);
    for (size_t byte = 0; byte < size; byte++)
        for (unsigned bit = 0; bit < 8; bit++) {
            memcpy(changed, seed, size);
            changed[byte] ^= (unsigned char)(1u << bit);
            exercise(changed, size);
        }
    /* Recompute checksums to reach semantic parsing beyond CRC rejection. */
    for (size_t chunk = 0; chunk < chunks; chunk++)
        for (size_t byte = 0; byte < lengths[chunk]; byte++) {
            memcpy(changed, seed, size);
            changed[starts[chunk] + 12 + byte] ^= 0xff;
            ps_put_u32(changed + starts[chunk] + 8,
                       ps_crc32(changed + starts[chunk] + 12, lengths[chunk]));
            exercise(changed, size);
        }
    const uint32_t extremes[] = {0, 1, 8192, 8193, UINT32_MAX};
    for (size_t chunk = 0; chunk < chunks; chunk++)
        for (unsigned i = 0; i < 5; i++) {
            memcpy(changed, seed, size);
            ps_put_u32(changed + starts[chunk] + 4, extremes[i]);
            exercise(changed, size);
        }
    require(opened > 100 && rows > 100);
    remove("run-mutation.psrun");
    remove("run-seed.psrun");
    printf("Run mutation campaign: %llu cases, %llu opened, %llu valid rows\n", cases, opened,
           rows);
    return 0;
}
