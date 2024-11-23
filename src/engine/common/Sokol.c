#include "Sokol.h"

#include "../Engine.h"  // IWYU pragma: keep

// sokol_log

extern void slog_func(
    const char* tag,
    u32 log_level,
    u32 log_item,
    const char* message,
    u32 line_nr,
    const char* filename,
    void* user_data);

void wslog_func(
    const char* tag,
    u32 log_level,
    u32 log_item,
    const char* message,
    u32 line_nr,
    const char* filename,
    void* user_data) {
  slog_func(tag, log_level, log_item, message, line_nr, filename, user_data);
}

// sokol_app

extern void app_lock_mouse(bool lock);

void wsapp_lock_mouse(bool lock) {
  if (!g_engine->useInput) return;
  sapp_lock_mouse(lock);
}

extern bool sapp_mouse_locked(void);

bool wsapp_mouse_locked(void) {
  if (!g_engine->useInput) return false;
  return sapp_mouse_locked();
}

extern void sapp_update_window_title(const char* title);

void wsapp_update_window_title(const char* title) {
  if (!g_engine->useVideo) return;
#ifdef __EMSCRIPTEN__
  EM_ASM({ document.title = UTF8ToString($0); }, title);
  EM_ASM({ console.debug(UTF8ToString($0)); }, title);
#else
  sapp_set_window_title(title);
#endif
}

extern void sapp_request_quit(void);

void wsapp_request_quit(void) {
  if (g_engine->useVideo) {
    sapp_request_quit();
  }
  g_engine->quit = true;
}

// sokol_gfx

extern void sg_setup(const sg_desc* desc);

void wsg_setup(const sg_desc* desc) {
  if (!g_engine->useVideo) return;
  sg_setup(desc);
}

extern sg_environment sglue_environment(void);

sg_environment wsglue_environment(void) {
  if (!g_engine->useVideo) return (sg_environment){};
  return sglue_environment();
}

extern sg_swapchain sglue_swapchain(void);

sg_swapchain wsglue_swapchain(void) {
  if (!g_engine->useVideo) return (sg_swapchain){};
  return sglue_swapchain();
}

extern sg_buffer sg_make_buffer(const sg_buffer_desc* desc);

sg_buffer wsg_make_buffer(const sg_buffer_desc* desc) {
  if (!g_engine->useVideo) return (sg_buffer){};
  return sg_make_buffer(desc);
}

extern sg_shader sg_make_shader(const sg_shader_desc* desc);

sg_shader wsg_make_shader(const sg_shader_desc* desc) {
  if (!g_engine->useVideo) return (sg_shader){};
  return sg_make_shader(desc);
}

extern sg_backend sg_query_backend(void);

sg_backend wsg_query_backend(void) {
  if (!g_engine->useVideo) return (sg_backend){};
  return sg_query_backend();
}

extern sg_pipeline sg_make_pipeline(const sg_pipeline_desc* desc);

sg_pipeline wsg_make_pipeline(const sg_pipeline_desc* desc) {
  if (!g_engine->useVideo) return (sg_pipeline){};
  return sg_make_pipeline(desc);
}

extern void sg_begin_pass(const sg_pass* pass);

void wsg_begin_pass(const sg_pass* pass) {
  if (!g_engine->useVideo) return;
  sg_begin_pass(pass);
}

extern void sg_apply_pipeline(sg_pipeline pip);

void wsg_apply_pipeline(sg_pipeline pip) {
  if (!g_engine->useVideo) return;
  sg_apply_pipeline(pip);
}

extern void sg_apply_bindings(const sg_bindings* bindings);

void wsg_apply_bindings(const sg_bindings* bindings) {
  if (!g_engine->useVideo) return;
  sg_apply_bindings(bindings);
}

extern void sg_draw(int base_element, int num_elements, int num_instances);

void wsg_draw(int base_element, int num_elements, int num_instances) {
  if (!g_engine->useVideo) return;
  sg_draw(base_element, num_elements, num_instances);
}

extern void sg_end_pass(void);

void wsg_end_pass(void) {
  if (!g_engine->useVideo) return;
  sg_end_pass();
}

extern void sg_commit(void);

void wsg_commit(void) {
  if (!g_engine->useVideo) return;
  sg_commit();
}

extern void sg_shutdown(void);

void wsg_shutdown(void) {
  if (!g_engine->useVideo) return;
  sg_shutdown();
}

extern sg_image sg_alloc_image(void);

sg_image wsg_alloc_image(void) {
  if (!g_engine->useVideo) return (sg_image){};
  return sg_alloc_image();
}

extern sg_sampler sg_alloc_sampler(void);

sg_sampler wsg_alloc_sampler(void) {
  if (!g_engine->useVideo) return (sg_sampler){};
  return sg_alloc_sampler();
}

extern void sg_init_sampler(sg_sampler smp_id, const sg_sampler_desc* desc);

void wsg_init_sampler(sg_sampler smp_id, const sg_sampler_desc* desc) {
  if (!g_engine->useVideo) return;
  sg_init_sampler(smp_id, desc);
}

extern void sg_apply_uniforms(sg_shader_stage stage, int ub_index, const sg_range* data);

void wsg_apply_uniforms(sg_shader_stage stage, int ub_index, const sg_range* data) {
  if (!g_engine->useVideo) return;
  sg_apply_uniforms(stage, ub_index, data);
}

extern void sg_init_image(sg_image img_id, const sg_image_desc* desc);

void wsg_init_image(sg_image img_id, const sg_image_desc* desc) {
  if (!g_engine->useVideo) return;
  sg_init_image(img_id, desc);
}

extern void sg_update_buffer(sg_buffer buf_id, const sg_range* data);

void wsg_update_buffer(sg_buffer buf_id, const sg_range* data) {
  if (!g_engine->useVideo) return;
  sg_update_buffer(buf_id, data);
}

extern void sg_update_image(sg_image img_id, const sg_image_data* data);

void wsg_update_image(sg_image img_id, const sg_image_data* data) {
  if (!g_engine->useVideo) return;
  sg_update_image(img_id, data);
}

extern sg_frame_stats sg_query_frame_stats(void);

sg_frame_stats wsg_query_frame_stats(void) {
  if (!g_engine->useVideo) return (sg_frame_stats){};
  return sg_query_frame_stats();
}

extern void sg_enable_frame_stats(void);

void wsg_enable_frame_stats(void) {
  if (!g_engine->useVideo) return;
  sg_enable_frame_stats();
}

extern void sg_disable_frame_stats(void);

void wsg_disable_frame_stats(void) {
  if (!g_engine->useVideo) return;
  sg_disable_frame_stats();
}

extern bool sg_frame_stats_enabled(void);

bool wsg_frame_stats_enabled(void) {
  if (!g_engine->useVideo) return false;
  return sg_frame_stats_enabled();
  return false;
}

extern sg_image sg_make_image(const sg_image_desc* desc);

sg_image wsg_make_image(const sg_image_desc* desc) {
  if (!g_engine->useVideo) return (sg_image){};
  return sg_make_image(desc);
}

extern sg_attachments sg_make_attachments(const sg_attachments_desc* desc);

sg_attachments wsg_make_attachments(const sg_attachments_desc* desc) {
  if (!g_engine->useVideo) return (sg_attachments){};
  return sg_make_attachments(desc);
}

// sokol_audio

extern void saudio_setup(const saudio_desc* desc);

void wsaudio_setup(const saudio_desc* desc) {
  if (!g_engine->useAudio) return;
  saudio_setup(desc);
}

extern int saudio_sample_rate(void);

int wsaudio_sample_rate(void) {
  if (!g_engine->useAudio) return 0;
  return saudio_sample_rate();
  return 0;
}

extern int saudio_channels(void);

int wsaudio_channels(void) {
  if (!g_engine->useAudio) return 0;
  return saudio_channels();
  return 0;
}

extern void saudio_shutdown(void);

void wsaudio_shutdown(void) {
  if (!g_engine->useAudio) return;
  saudio_shutdown();
}

// sokol_time

extern void stm_setup(void);

void wstm_setup(void) {
  if (!g_engine->useTime) return;
  stm_setup();
}

extern u64 stm_now(void);

u64 wstm_now(void) {
  if (!g_engine->useTime) return 0;
  return stm_now();
}

extern f64 stm_ns(u64 ticks);

f64 wstm_ns(u64 ticks) {
  if (!g_engine->useTime) return 0;
  return stm_ns(ticks);
}

extern f64 stm_us(u64 ticks);

f64 wstm_us(u64 ticks) {
  if (!g_engine->useTime) return 0;
  return stm_us(ticks);
}

extern f64 stm_ms(u64 ticks);

f64 wstm_ms(u64 ticks) {
  if (!g_engine->useTime) return 0;
  return stm_ms(ticks);
}

extern f64 stm_sec(u64 ticks);

f64 wstm_sec(u64 ticks) {
  if (!g_engine->useTime) return 0;
  return stm_sec(ticks);
}

extern u64 stm_laptime(u64* last_time);

u64 wstm_laptime(u64* last_time) {
  if (!g_engine->useTime) return 0;
  return stm_laptime(last_time);
}

// sokol_fetch

extern void sfetch_setup(const sfetch_desc_t* desc);

void wsfetch_setup(const sfetch_desc_t* desc) {
  return sfetch_setup(desc);
}

extern void sfetch_dowork(void);

void wsfetch_dowork(void) {
  sfetch_dowork();
}

extern void sfetch_shutdown(void);

void wsfetch_shutdown(void) {
  sfetch_shutdown();
}

extern sfetch_handle_t sfetch_send(const sfetch_request_t* request);

sfetch_handle_t wsfetch_send(const sfetch_request_t* request) {
  return sfetch_send(request);
}
