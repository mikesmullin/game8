#pragma once

// OS detection via compiler context

#ifdef __clang__
#define COMPILER_CLANG 1
#else
#define COMPILER_CLANG 0
#error compiler not supported
#endif

#ifdef _WIN32
#define OS_WINDOWS 1
#else
#define OS_WINDOWS 0
#ifdef __linux__
#ifdef __ANDROID__
#define OS_LINUX 0
#define OS_ANDROID 1
#else
#define OS_ANDROID 0
#define OS_LINUX 1
#endif
#else
#define OS_LINUX 0
#ifdef __APPLE__
#define OS_MAC 1
#else
#define OS_MAC 0
#error os not supported
#endif
#endif
#endif

#ifdef __amd64__
#define ARCH_X64 1
#define ARCH_X86 0
#else
#define ARCH_X64 0
#ifdef __i386__
#define ARCH_X86 1
#define ARCH_X64 0
#else
#define ARCH_X86 0
#ifdef __aarch64__
#define ARCH_ARM64 1
#else
#define ARCH_ARM64 0
#error arch not supported
#endif
#endif
#endif
