#pragma once

#include "../../../vendor/sokol/sokol_app.h"
#include "../../../vendor/sokol/sokol_gfx.h"

//

#include "../../../vendor/sokol/sokol_audio.h"
#include "../../../vendor/sokol/sokol_fetch.h"
#include "../../../vendor/sokol/sokol_glue.h"
#include "../../../vendor/sokol/sokol_log.h"
#include "../../../vendor/sokol/sokol_time.h"

//

#include "Types.h"

// sokol_log

void wslog_func(
    const char* tag,
    u32 log_level,
    u32 log_item,
    const char* message,
    u32 line_nr,
    const char* filename,
    void* user_data);

// sokol_app

void wsapp_lock_mouse(bool lock);
bool wsapp_mouse_locked(void);
void wsapp_update_window_title(const char* title);

// sokol_gfx

void wsg_setup(const sg_desc* desc);
sg_environment wsglue_environment(void);
sg_swapchain wsglue_swapchain(void);
sg_buffer wsg_make_buffer(const sg_buffer_desc* desc);
sg_shader wsg_make_shader(const sg_shader_desc* desc);
sg_backend wsg_query_backend(void);
sg_pipeline wsg_make_pipeline(const sg_pipeline_desc* desc);
void wsg_begin_pass(const sg_pass* pass);
void wsg_apply_pipeline(sg_pipeline pip);
void wsg_apply_bindings(const sg_bindings* bindings);
void wsg_draw(int base_element, int num_elements, int num_instances);
void wsg_end_pass(void);
void wsg_commit(void);
void wsg_shutdown(void);
sg_image wsg_alloc_image(void);
sg_sampler wsg_alloc_sampler(void);
void wsg_init_sampler(sg_sampler smp_id, const sg_sampler_desc* desc);
void wsg_apply_uniforms(sg_shader_stage stage, int ub_index, const sg_range* data);
void wsg_init_image(sg_image img_id, const sg_image_desc* desc);
void wsg_update_buffer(sg_buffer buf_id, const sg_range* data);
void wsg_update_image(sg_image img_id, const sg_image_data* data);
sg_frame_stats wsg_query_frame_stats(void);
void wsg_enable_frame_stats(void);
void wsg_disable_frame_stats(void);
bool wsg_frame_stats_enabled(void);
sg_image wsg_make_image(const sg_image_desc* desc);
sg_attachments wsg_make_attachments(const sg_attachments_desc* desc);

// sokol_audio

void wsaudio_setup(const saudio_desc* desc);
int wsaudio_sample_rate(void);
int wsaudio_channels(void);
void wsaudio_shutdown(void);

// sokol_time

void wstm_setup(void);
u64 wstm_now(void);
f64 wstm_ns(u64 ticks);
f64 wstm_us(u64 ticks);
f64 wstm_ms(u64 ticks);
f64 wstm_sec(u64 ticks);
u64 wstm_laptime(u64* last_time);

// sokol_fetch

void wsfetch_setup(const sfetch_desc_t* desc);
void wsfetch_dowork(void);
void wsfetch_shutdown(void);
sfetch_handle_t wsfetch_send(const sfetch_request_t* request);
