#ifndef PHYSIM_DETAIL_BUILD_ORIGIN_H
#define PHYSIM_DETAIL_BUILD_ORIGIN_H
#define PS_BUILD_TEXT_IMPL(value) #value
#define PS_BUILD_TEXT(value) PS_BUILD_TEXT_IMPL(value)
#if defined(__clang__)
#define PS_BUILD_COMPILER "Clang " __clang_version__
#elif defined(_MSC_FULL_VER)
#define PS_BUILD_COMPILER "MSVC " PS_BUILD_TEXT(_MSC_FULL_VER)
#elif defined(__GNUC__)
#define PS_BUILD_COMPILER "GCC " __VERSION__
#else
#define PS_BUILD_COMPILER "unavailable"
#endif
/* Compiler's literal build date/time; no runtime clock or timezone claim. */
#define PS_BUILD_STAMP __DATE__ " " __TIME__
#endif
