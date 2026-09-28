#ifndef PHYSIM_BENCHMARK_BUILD_H
#define PHYSIM_BENCHMARK_BUILD_H

#define PS_BENCH_STRING_IMPL(value) #value
#define PS_BENCH_STRING(value) PS_BENCH_STRING_IMPL(value)

#if defined(__clang__)
#if defined(__apple_build_version__)
#define PS_BENCH_FAMILY "AppleClang-"
#elif defined(_MSC_VER)
#define PS_BENCH_FAMILY "ClangCL-"
#else
#define PS_BENCH_FAMILY "Clang-"
#endif
#define PS_BENCH_COMPILER PS_BENCH_FAMILY PS_BENCH_STRING(__clang_major__) "." \
    PS_BENCH_STRING(__clang_minor__) "." PS_BENCH_STRING(__clang_patchlevel__)
#elif defined(_MSC_FULL_VER)
#define PS_BENCH_COMPILER "MSVC-" PS_BENCH_STRING(_MSC_FULL_VER)
#elif defined(__GNUC__)
#define PS_BENCH_COMPILER "GCC-" PS_BENCH_STRING(__GNUC__) "." \
    PS_BENCH_STRING(__GNUC_MINOR__) "." PS_BENCH_STRING(__GNUC_PATCHLEVEL__)
#else
#error Unsupported benchmark compiler
#endif

#ifndef PS_BENCH_CONFIG
#ifdef NDEBUG
#define PS_BENCH_CONFIG "Release"
#else
#define PS_BENCH_CONFIG "Debug"
#endif
#endif

#endif
