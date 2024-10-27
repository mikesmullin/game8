#pragma once

// #define PROFILER__INSTRUMENTED
#ifdef PROFILER__INSTRUMENTED
#define PROFILE__BEGIN(id) Profiler__beginTrace(id)
#define PROFILE__END(id) Profiler__endTrace(id)
#define PROFILE__PRINT() Profiler__printf()
#else
#define PROFILE__BEGIN(id)
#define PROFILE__END(id)
#define PROFILE__PRINT()
#endif

void Profiler__beginTrace(int id);
void Profiler__endTrace(int id);
void Profiler__printf();
