#if defined(_MSC_VER)
#include <intrin.h>
#define DEBUGGER __debugbreak()  // MSVC

#elif defined(__GNUC__) || defined(__clang__)
#if defined(__x86_64__) || defined(__i386__)
#define DEBUGGER __asm__("int3")  // GCC/Clang x86/x86_64

#else
#include <signal.h>
#define DEBUGGER raise(SIGTRAP)  // GCC/Clang on non-x86 platforms
#endif

#else
#error "Unsupported compiler for Breakpoint feature"
#endif