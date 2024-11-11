#include "Profiler.h"

#include "../Engine.h"
#include "Dispatcher.h"
#include "Types.h"

typedef struct Trace {
  u64 last_time;
  u64 total_time;
  u32 call_count;
} Trace;

typedef struct Profiler {
  Trace traces[DISPATCH1__COUNT];
} Profiler;

static Profiler profiler;

void Profiler__beginTrace(int id) {
  u64 now = g_engine->stm_now();
  profiler.traces[id].last_time = -now;
  profiler.traces[id].call_count++;
}

void Profiler__endTrace(int id) {
  u64 now = g_engine->stm_now();
  profiler.traces[id].last_time += now;
  profiler.traces[id].total_time += profiler.traces[id].last_time;
}

void Profiler__printf() {
  LOG_DEBUGF("\nProfiler:")
  for (u32 id = 0; id < DISPATCH1__COUNT; id++) {
    if (0 == profiler.traces[id].call_count) continue;
    LOG_DEBUGF(
        "  fn %2u took %5llu ticks %5.1lf us %5.1lf ms avg (%5d calls)",
        id,
        profiler.traces[id].total_time / profiler.traces[id].call_count,
        g_engine->stm_us(profiler.traces[id].total_time) / profiler.traces[id].call_count,
        g_engine->stm_ms(profiler.traces[id].total_time) / profiler.traces[id].call_count,
        profiler.traces[id].call_count);
  }
}