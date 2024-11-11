#pragma once

#ifdef ENGINE__MAIN
#define SOKOL_IMPL
#endif
#ifdef ENGINE__DLL
#define SOKOL_IMPL
#define SOKOL_NO_ENTRY
#endif
#ifdef ENGINE__NO_MAIN
#define SOKOL_IMPL
#define SOKOL_NO_ENTRY
#endif
#include "common/Sokol.h"

//

#include "common/Arena.h"  // IWYU pragma: keep
#include "common/HotReload.h"  // IWYU pragma: keep
#include "common/Log.h"  // IWYU pragma: keep
#include "common/Math.h"  // IWYU pragma: keep
#include "common/Types.h"
#include "common/Utils.h"  // IWYU pragma: keep

typedef struct Engine__State Engine__State;

typedef void (*logic_oninit_t)(Engine__State* state);
typedef void (*logic_onpreload_t)(void);
typedef void (*logic_onreload_t)(Engine__State* state);
typedef void (*logic_onevent_t)(const sapp_event* event);
typedef void (*logic_onfixedupdate_t)(void);
typedef void (*logic_onupdate_t)(void);
typedef void (*logic_onshutdown_t)(void);

typedef struct Logic__State Logic__State;
typedef struct WavReader WavReader;

typedef struct Engine__State {
  char window_title[255];
  bool isMaster;
  bool useVideo;
  bool useAudio;
  bool useInput;
  bool useNet;
  bool useTime;
  bool useHotReload;

  FileMonitor fm;
  bool quit;

  u32 window_width, window_height;
  Arena *arena, *frameArena;
  Logic__State* logic;
  void (*stream_cb1)(float* buffer, int num_frames, int num_channels);
  void (*stream_cb2)(float* buffer, int num_frames, int num_channels);

  void (*log)(const char* line, ...);
  void (*abort)(const char* line, ...);

  void (*sg_setup)(const sg_desc* desc);
  sg_environment (*sglue_environment)(void);
  sg_swapchain (*sglue_swapchain)(void);
  void (*slog_func)(
      const char* tag,
      uint32_t log_level,
      uint32_t log_item,
      const char* message,
      uint32_t line_nr,
      const char* filename,
      void* user_data);

  void (*sapp_lock_mouse)(bool lock);
  bool (*sapp_mouse_locked)(void);
  void (*sapp_update_window_title)(const char* title);

  sg_buffer (*sg_make_buffer)(const sg_buffer_desc* desc);
  sg_shader (*sg_make_shader)(const sg_shader_desc* desc);
  sg_backend (*sg_query_backend)(void);
  sg_pipeline (*sg_make_pipeline)(const sg_pipeline_desc* desc);
  void (*sg_begin_pass)(const sg_pass* pass);
  void (*sg_apply_pipeline)(sg_pipeline pip);
  void (*sg_apply_bindings)(const sg_bindings* bindings);
  void (*sg_draw)(int base_element, int num_elements, int num_instances);
  void (*sg_end_pass)(void);
  void (*sg_commit)(void);
  void (*sg_shutdown)(void);
  sg_image (*sg_alloc_image)(void);
  sg_sampler (*sg_alloc_sampler)(void);
  void (*sg_init_sampler)(sg_sampler smp_id, const sg_sampler_desc* desc);
  void (*sg_apply_uniforms)(sg_shader_stage stage, int ub_index, const sg_range* data);
  void (*sg_init_image)(sg_image img_id, const sg_image_desc* desc);
  void (*sg_update_buffer)(sg_buffer buf_id, const sg_range* data);
  void (*sg_update_image)(sg_image img_id, const sg_image_data* data);
  sg_frame_stats (*sg_query_frame_stats)(void);
  void (*sg_enable_frame_stats)(void);
  void (*sg_disable_frame_stats)(void);
  bool (*sg_frame_stats_enabled)(void);
  sg_image (*sg_make_image)(const sg_image_desc* desc);
  sg_attachments (*sg_make_attachments)(const sg_attachments_desc* desc);

  void (*saudio_setup)(const saudio_desc* desc);
  int (*saudio_sample_rate)(void);
  int (*saudio_channels)(void);
  void (*saudio_shutdown)(void);

  void (*stm_setup)(void);
  uint64_t (*stm_now)(void);
  double (*stm_ns)(uint64_t ticks);
  double (*stm_us)(uint64_t ticks);
  double (*stm_ms)(uint64_t ticks);
  double (*stm_sec)(uint64_t ticks);
  uint64_t (*stm_laptime)(uint64_t* last_time);

  void (*sfetch_setup)(const sfetch_desc_t* desc);
  void (*sfetch_dowork)(void);
  void (*sfetch_shutdown)(void);
  sfetch_handle_t (*sfetch_send)(const sfetch_request_t* request);

  logic_oninit_t logic_oninit;
  logic_onpreload_t logic_onpreload;
  logic_onreload_t logic_onreload;
  logic_onevent_t logic_onevent;
  logic_onfixedupdate_t logic_onfixedupdate;
  logic_onupdate_t logic_onupdate;
  logic_onshutdown_t logic_onshutdown;

  u64 now;
  f32 deltaTime;
  u16 entity_count;
  u16 draw_count;

  WavReader* aSrc;  // current audio source

} Engine__State;

#ifndef ENGINE__COUNT
#define ENGINE__COUNT (1)
#endif
#if defined(ENGINE__MAIN) || defined(ENGINE__NO_MAIN)
Engine__State engines[ENGINE__COUNT];
#endif

#if defined(ENGINE__MAIN) && defined(ENGINE__HOT_RELOAD)  // app.c (game)
Engine__State* g_engine;
#endif

#if defined(ENGINE__MAIN) && !defined(ENGINE__HOT_RELOAD)  // app.c (integration test)
Engine__State* g_engine;
#endif

#if defined(ENGINE__DLL) && !defined(ENGINE__HOT_RELOAD)  // Logic.c.dll (alone)
Engine__State* g_engine;
#endif

#if defined(ENGINE__NO_MAIN) && !defined(ENGINE__HOT_RELOAD)  // app.c (unit test)
Engine__State* g_engine;
#endif

#if (!defined(ENGINE__MAIN) && !defined(ENGINE__DLL))  // all other translation units
extern Engine__State* g_engine;
#endif

void Engine__init();
void Engine__cli(int argc, char* argv[]);
sapp_desc Engine__sokol_main(int argc, char* argv[]);
void Engine__sokol_init(void);
void Engine__sokol_frame(void);
void Engine__sokol_cleanup(void);
void Engine__sokol_event_cb(const sapp_event* event);
void Engine__sokol_stream_cb(float* buffer, int num_frames, int num_channels);
f32 mwave(f32 ms, f32 a, f32 b);
