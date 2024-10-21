#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef int8_t s8;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int32_t s32;
typedef int64_t s64;
typedef float f32;
typedef double f64;

typedef struct Arena Arena;
typedef struct Logic__State Logic__State;
typedef struct sg_environment sg_environment;
typedef struct sg_swapchain sg_swapchain;
typedef struct sg_buffer sg_buffer;
typedef struct sg_buffer_desc sg_buffer_desc;
typedef struct sg_shader sg_shader;
typedef struct sg_shader_desc sg_shader_desc;
typedef enum sg_backend sg_backend;
typedef struct sg_pipeline sg_pipeline;
typedef struct sg_pipeline_desc sg_pipeline_desc;
typedef struct saudio_desc saudio_desc;
typedef struct sg_pass sg_pass;
typedef struct sg_pipeline sg_pipeline;
typedef struct sg_desc sg_desc;
typedef struct sg_bindings sg_bindings;
typedef struct sg_pass_action sg_pass_action;
typedef struct sfetch_desc_t sfetch_desc_t;
typedef struct sfetch_handle_t sfetch_handle_t;
typedef struct sfetch_request_t sfetch_request_t;

typedef struct Engine__State {
  char* window_title;
  u32 window_width, window_height;
  Arena* arena;
  Logic__State* logic;
  void (*stream_cb1)(float* buffer, int num_frames, int num_channels);
  void (*stream_cb2)(float* buffer, int num_frames, int num_channels);

  void (*stm_setup)(void);
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
  sg_buffer (*sg_make_buffer)(const sg_buffer_desc* desc);
  sg_shader (*sg_make_shader)(const sg_shader_desc* desc);
  sg_backend (*sg_query_backend)(void);
  sg_pipeline (*sg_make_pipeline)(const sg_pipeline_desc* desc);
  void (*saudio_setup)(const saudio_desc* desc);
  int (*saudio_sample_rate)(void);
  int (*saudio_channels)(void);
  double (*stm_ms)(uint64_t ticks);
  uint64_t (*stm_now)(void);
  void (*sg_begin_pass)(const sg_pass* pass);
  void (*sg_apply_pipeline)(sg_pipeline pip);
  void (*sg_apply_bindings)(const sg_bindings* bindings);
  void (*sg_draw)(int base_element, int num_elements, int num_instances);
  void (*sg_end_pass)(void);
  void (*sg_commit)(void);
  void (*sg_shutdown)(void);
  void (*saudio_shutdown)(void);
  void (*sfetch_setup)(const sfetch_desc_t* desc);
  void (*sfetch_dowork)(void);
  void (*sfetch_shutdown)(void);
  sfetch_handle_t (*sfetch_send)(const sfetch_request_t* request);
  void (*log)(const char* line, ...);

} Engine__State;

#ifdef __EMSCRIPTEN__
void logic_oninit(Engine__State* state);
void logic_onpreload(void);
void logic_onreload(Engine__State* state);
void logic_onfixedupdate(void);
void logic_onupdate(void);
void logic_onshutdown(void);
#else
typedef void (*logic_oninit_t)(Engine__State* state);
typedef void (*logic_onpreload_t)(void);
typedef void (*logic_onreload_t)(Engine__State* state);
typedef void (*logic_onfixedupdate_t)(void);
typedef void (*logic_onupdate_t)(void);
typedef void (*logic_onshutdown_t)(void);
#endif

typedef struct WavReader WavReader;
typedef struct Wavefront Wavefront;

typedef struct Logic__State {
  u64 now;

  WavReader* wr;
  Wavefront* wf;

  sg_pipeline* pip;
  sg_bindings* bind;
  sg_pass_action* pass_action;
} Logic__State;