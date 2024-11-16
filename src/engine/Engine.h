#pragma once

#if defined(ENGINE__MAIN) || defined(ENGINE__DLL) || defined(ENGINE__NO_MAIN)
#define SOKOL_IMPL
#define SOKOL_NO_ENTRY
#endif
#include "common/Sokol.h"

// Convenience --------------------------------------------

#include "entities/Entity.h"  // IWYU pragma: keep

//

typedef void (*Dispatcher__call2_t)(u32 id, Entity* inst, void* params);

#include "common/Arena.h"
#include "common/Audio.h"  // IWYU pragma: keep
#include "common/BehaviorTree.h"  // IWYU pragma: keep
#include "common/Bmp.h"  // IWYU pragma: keep
#include "common/Color.h"  // IWYU pragma: keep
#include "common/Easing.h"  // IWYU pragma: keep
#include "common/EventEmitter.h"  // IWYU pragma: keep
#include "common/HotReload.h"
#include "common/List.h"
#include "common/Log.h"  // IWYU pragma: keep
#include "common/Math.h"  // IWYU pragma: keep
#include "common/Net.h"  // IWYU pragma: keep
#include "common/Preloader.h"
#include "common/Profiler.h"  // IWYU pragma: keep
#include "common/QuadTree.h"
#include "common/Sleep.h"  // IWYU pragma: keep
#include "common/StateGraph.h"  // IWYU pragma: keep
#include "common/Types.h"
#include "common/Utils.h"  // IWYU pragma: keep
#include "common/Wav.h"  // IWYU pragma: keep
#include "common/Wavefront.h"  // IWYU pragma: keep
#include "common/Websocket.h"  // IWYU pragma: keep

// Components ----------------------------------------------

#include "components/AudioListener.h"  // IWYU pragma: keep
#include "components/AudioSource.h"  // IWYU pragma: keep
#include "components/Camera.h"  // IWYU pragma: keep
#include "components/Collider.h"  // IWYU pragma: keep
#include "components/Dispatch.h"  // IWYU pragma: keep
#include "components/GameInput.h"  // IWYU pragma: keep
#include "components/Health.h"  // IWYU pragma: keep
#include "components/MeshRenderer.h"  // IWYU pragma: keep
#include "components/Render.h"  // IWYU pragma: keep
#include "components/Rigidbody2D.h"  // IWYU pragma: keep
#include "components/Transform.h"  // IWYU pragma: keep

typedef struct Engine__State Engine__State;
typedef struct Game Game;
typedef struct PreloadedAudio PreloadedAudio;
typedef struct PreloadedModels PreloadedModels;
typedef struct PreloadedTextures PreloadedTextures;
typedef struct PreloadedMaterials PreloadedMaterials;

typedef struct Engine__State {
  char window_title[255];
  bool isMaster;
  bool useVideo;
  bool useAudio;
  bool useInput;
  bool useNet;
  bool useTime;
  bool useHotReload;

  void (*onbootstrap)(Engine__State* engine);
  void (*oninit)(void);
  void (*onpreload)(void);
  void (*onevent)(const sapp_event* event);
  void (*onfixedupdate)(void);
  void (*onupdate)(void);
  void (*onshutdown)(void);

  FileMonitor fm;
  bool quit;

  u32 window_width, window_height;
  Arena *arena, *frameArena;
  Game* game;
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
  void (*sapp_request_quit)(void);

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

  u64 now;
  f32 deltaTime;
  u16 entity_count;
  u16 draw_count;

  WavReader* aSrc;  // current audio source
  u32 lastUid;
  List* players;
  PreloadedAudio* audio;
  PreloadedModels* models;
  PreloadedTextures* textures;
  PreloadedMaterials* materials;

} Engine__State;

#ifndef ENGINE__COUNT
#define ENGINE__COUNT (1)
#endif
#if defined(ENGINE__MAIN) || defined(ENGINE__NO_MAIN)
Engine__State engines[ENGINE__COUNT];
Engine__State* g_engine = &engines[0];
#endif

#if defined(ENGINE__DLL)
Engine__State* g_engine;
#endif

#if (!defined(ENGINE__MAIN) && !defined(ENGINE__DLL))  // all other translation units
extern Engine__State* g_engine;
#endif

void Engine__init();
void Engine__cli(int argc, char* argv[]);
int Engine__main(int argc, char* argv[]);
sapp_desc Engine__sokol_main(int argc, char* argv[]);
void Engine__sokol_init(void);
void Engine__sokol_frame(void);
void Engine__sokol_cleanup(void);
void Engine__sokol_event_cb(const sapp_event* event);
void Engine__sokol_stream_cb(float* buffer, int num_frames, int num_channels);
f32 mwave(f32 ms, f32 a, f32 b);
