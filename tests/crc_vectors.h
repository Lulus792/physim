/* Frozen independent oracle: Python zlib.crc32; data[i] = (i*31) ^ (i>>8) ^ 0xa5. */
#include <stddef.h>
#include <stdint.h>
static const struct {
    size_t size;
    uint32_t crc;
} golden[] = {
    {0u, UINT32_C(0x00000000)},       {1u, UINT32_C(0x74beb8ea)},
    {2u, UINT32_C(0xb91d00c1)},       {3u, UINT32_C(0x59054a67)},
    {7u, UINT32_C(0x983f23da)},       {8u, UINT32_C(0xed2fd673)},
    {9u, UINT32_C(0x0e39cd94)},       {15u, UINT32_C(0x604bf111)},
    {16u, UINT32_C(0xefba31ab)},      {31u, UINT32_C(0x399e9aa6)},
    {32u, UINT32_C(0xa751d28b)},      {55u, UINT32_C(0x556bf8b7)},
    {56u, UINT32_C(0x8ee4ee71)},      {63u, UINT32_C(0xc94a740f)},
    {64u, UINT32_C(0x45197c71)},      {127u, UINT32_C(0x53e59a7f)},
    {128u, UINT32_C(0x8ee26013)},     {135u, UINT32_C(0x6478070c)},
    {136u, UINT32_C(0x6fdb6596)},     {255u, UINT32_C(0x337048cc)},
    {256u, UINT32_C(0x315294d7)},     {257u, UINT32_C(0x1b3f9d9f)},
    {1023u, UINT32_C(0x544f989b)},    {4095u, UINT32_C(0x1186f84b)},
    {4096u, UINT32_C(0xd2136975)},    {8191u, UINT32_C(0x980b956a)},
    {8192u, UINT32_C(0x8344e422)},    {65535u, UINT32_C(0xd0b912af)},
    {65536u, UINT32_C(0xc80882e2)},   {1048576u, UINT32_C(0x415e9ed5)},
    {8388608u, UINT32_C(0x0f365382)},
};
