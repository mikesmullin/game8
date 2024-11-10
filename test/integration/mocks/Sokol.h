#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "../../../vendor/sokol/sokol_fetch.h"
#include "../../../vendor/sokol/sokol_time.h"

// sokol_gfx

typedef struct sg_environment {
} sg_environment;
typedef struct sg_swapchain {
} sg_swapchain;
typedef struct sg_buffer {
} sg_buffer;
typedef struct sg_buffer_desc {
} sg_buffer_desc;
typedef struct sg_shader {
} sg_shader;
typedef struct sg_shader_desc {
} sg_shader_desc;
typedef enum sg_backend {
  SG_BE_NONE__TMP,
} sg_backend;
typedef struct sg_pipeline {
} sg_pipeline;
typedef struct sg_pipeline_desc {
} sg_pipeline_desc;
typedef struct saudio_desc {
} saudio_desc;
typedef struct sg_pass {
} sg_pass;
typedef struct sg_desc {
} sg_desc;
typedef struct sg_bindings {
} sg_bindings;
typedef struct sg_pass_action {
} sg_pass_action;
typedef struct sg_image {
} sg_image;
typedef struct sg_sampler {
} sg_sampler;
typedef struct sg_sampler_desc {
} sg_sampler_desc;
typedef enum sg_shader_stage {
  SG_SHADER_NONE__TMP,
} sg_shader_stage;
typedef struct sg_range {
} sg_range;
typedef struct sg_image_desc {
} sg_image_desc;
typedef struct sg_image_data {
} sg_image_data;
typedef struct sg_frame_stats {
} sg_frame_stats;
typedef struct sg_attachments {
} sg_attachments;
typedef struct sg_attachments_desc {
} sg_attachments_desc;

void sg_setup(const sg_desc* desc);
sg_environment sglue_environment(void);
sg_swapchain sglue_swapchain(void);
void slog_func(
    const char* tag,
    uint32_t log_level,
    uint32_t log_item,
    const char* message,
    uint32_t line_nr,
    const char* filename,
    void* user_data);
sg_buffer sg_make_buffer(const sg_buffer_desc* desc);
sg_shader sg_make_shader(const sg_shader_desc* desc);
sg_backend sg_query_backend(void);
sg_pipeline sg_make_pipeline(const sg_pipeline_desc* desc);
void saudio_setup(const saudio_desc* desc);
int saudio_sample_rate(void);
int saudio_channels(void);
void sg_begin_pass(const sg_pass* pass);
void sg_apply_pipeline(sg_pipeline pip);
void sg_apply_bindings(const sg_bindings* bindings);
void sg_draw(int base_element, int num_elements, int num_instances);
void sg_end_pass(void);
void sg_commit(void);
void sg_shutdown(void);
void saudio_shutdown(void);
sg_image sg_alloc_image(void);
sg_sampler sg_alloc_sampler(void);
void sg_init_sampler(sg_sampler smp_id, const sg_sampler_desc* desc);
void sg_apply_uniforms(sg_shader_stage stage, int ub_index, const sg_range* data);
void sg_init_image(sg_image img_id, const sg_image_desc* desc);
void sg_update_buffer(sg_buffer buf_id, const sg_range* data);
void sg_update_image(sg_image img_id, const sg_image_data* data);
sg_frame_stats sg_query_frame_stats(void);
void sg_enable_frame_stats(void);
void sg_disable_frame_stats(void);
bool sg_frame_stats_enabled(void);
sg_image sg_make_image(const sg_image_desc* desc);
sg_attachments sg_make_attachments(const sg_attachments_desc* desc);

// sokol_app

typedef struct sapp_event {
} sapp_event;

void sapp_lock_mouse(bool lock);
bool sapp_mouse_locked(void);