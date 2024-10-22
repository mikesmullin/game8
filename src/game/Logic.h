#pragma once

// General ----------------------------------------------

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

typedef struct v2 {
  f32 x, y;
} v2;

typedef struct v3 {
  f32 x, y, z;
} v3;

typedef struct v4 {
  f32 x, y, z, w;
} v4;

typedef struct m4 {
  v4 a, b, c, d;
} m4;

// Engine ----------------------------------------------

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
typedef struct sg_image sg_image;
typedef struct sg_sampler sg_sampler;
typedef struct sg_sampler_desc sg_sampler_desc;
typedef enum sg_shader_stage sg_shader_stage;
typedef struct sg_range sg_range;
typedef struct sg_image_desc sg_image_desc;

typedef struct Engine__State {
  char* window_title;
  u32 window_width, window_height;
  Arena* arena;
  Logic__State* logic;
  void (*stream_cb1)(float* buffer, int num_frames, int num_channels);
  void (*stream_cb2)(float* buffer, int num_frames, int num_channels);

  void (*log)(const char* line, ...);
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
  uint64_t (*stm_now)(void);
  double (*stm_ms)(uint64_t ticks);
  double (*stm_sec)(uint64_t ticks);
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
  sg_image (*sg_alloc_image)(void);
  sg_sampler (*sg_alloc_sampler)(void);
  void (*sg_init_sampler)(sg_sampler smp_id, const sg_sampler_desc* desc);
  void (*sg_apply_uniforms)(sg_shader_stage stage, int ub_index, const sg_range* data);
  void (*sg_init_image)(sg_image img_id, const sg_image_desc* desc);

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

typedef struct QuadTreeNode QuadTreeNode;
typedef struct WavReader WavReader;
typedef struct Wavefront Wavefront;
typedef struct BmpReader BmpReader;
typedef struct List List;
typedef struct Entity Entity;
// typedef enum DispatchFnId : u32 DispatchFnId;
typedef struct KbInputState KbInputState;
typedef struct PointerInputState PointerInputState;

// Components ----------------------------------------------

#define MAX_LISTENERS 10

typedef enum EventType {
  EVENT_NONE,
  EVENT_HELLO,
  EVENT_GOODBYE,
  EVENT_DEATH,
  EVENT_ATTACKED,
  EVENT_ANIMOVER,
} EventType;

typedef enum ListenerFnId {
  BLOCK__HELLO,
  ENTITY__HELLO,
  CAT_ENTITY__GOODBYE,
  LISTENER_FN_COUNT
} ListenerFnId;

typedef struct EventEmitter {
  EventType event[MAX_LISTENERS];
  ListenerFnId listeners[MAX_LISTENERS];
  int count;
} EventEmitter;

typedef struct EngineComponent {
  u32 /*DispatchFnId*/ tick;
  u32 /*DispatchFnId*/ render;
  u32 /*DispatchFnId*/ gui;
} EngineComponent;

typedef struct TransformComponent {
  v3 pos;  // (x, y, z)
  v3 rot;  // (yaw, pitch, roll)
} TransformComponent;

typedef enum ColliderType {
  BOX_COLLIDER_2D,
  CIRCLE_COLLIDER_2D,
} ColliderType;

typedef struct OnCollideClosure_t {
  Entity *source, *target;
  f32 x, y;
  bool before, after;
  bool noclip;
} OnCollideParams;

typedef struct ColliderComponent {
  ColliderType type;
  u32 /*DispatchFnId*/ collide;
} ColliderComponent;

typedef struct BoxCollider2DComponent {
  ColliderComponent base;
  //f32 x, y; // origin
  f32 hw, hh;  // half width/height (radius)
} BoxCollider2DComponent;

typedef struct CircleCollider2DComponent {
  ColliderComponent base;
  //f32 x, y; // origin
  f32 r;  // radius
} CircleCollider2DComponent;

typedef struct Rigidbody2DComponent {
  f32 xa, za;  // movement deltas (pre-collision)
} Rigidbody2DComponent;

typedef enum RendererType {
  SPRITE_RENDERER,
  MESH_RENDERER,
} RendererType;

#define ATLAS_SPRITE_SZ (8)

typedef struct RendererComponent {
  RendererType type;
  Wavefront* mesh;
  BmpReader* texture;
  u32 tx, ty, ts;
  bool useMask;
  u32 mask, color;
  bool loaded;

  sg_pipeline* pip;
  sg_bindings* bind;
} RendererComponent;

// Entities ----------------------------------------------

typedef enum EntityTags1 : u64 {
  TAG_NONE = 0,
  TAG_LOADED = 1 << 1,
  TAG_WALL = 1 << 2,
  TAG_CAT = 1 << 3,
  TAG_BRICK = 1 << 4,
  TAG_BROKEN = 1 << 5,
} EntityTags1;

typedef struct Entity {
  u32 id;
  u64 tags1;
  EngineComponent* engine;
  TransformComponent* tform;
  // EventEmitterComponent* event;
  ColliderComponent* collider;
  Rigidbody2DComponent* rb;
  RendererComponent* render;
  // HealthComponent* health;
  // // TODO: if we don't need to iterate, these can be moved within subclass
  // AudioSourceComponent* audio;
  // AudioListenerComponent* hear;
  EventEmitter* events;
} Entity;

enum MODELS {
  MODEL_BOX = 0,  // models/box.obj
};

typedef struct Camera {
  f32 fov;  // field of view
  f32 nearZ;  // near plane
  f32 farZ;  // far plane
} Camera;

typedef struct VirtualJoystick {
  f32 xAxis, yAxis, zAxis;
} VirtualJoystick;

typedef struct Player {
  Entity base;
  Camera camera;
  VirtualJoystick input;
  f32 bobPhase;
} Player;

typedef struct Block {
  Entity base;
  enum MODELS meshId;
  bool masked;
} Block;

typedef struct WallBlock {
  Block base;
} WallBlock;

// typedef struct BreakBlock {
//   Block base;
//   RubbleSprite* sprites[32];
//   StateGraph* sg;
// } BreakBlock;

typedef struct SpawnBlock {
  Block base;
  bool firstTick;
} SpawnBlock;

// Game ----------------------------------------------

typedef struct Level {
  BmpReader* bmp;
  char* levelFile;
  bool skybox;
  BmpReader* world;
  char* worldFile;
  bool loaded;
  List* entities;
  u32 wallTex;
  u32 ceilTex;
  u32 floorTex;
  u32 wallCol;
  u32 ceilCol;
  u32 floorCol;
  u32 width;
  u32 depth;
  u32 height;
  SpawnBlock* spawner;
  Arena* qtArena;
  QuadTreeNode* qt;
} Level;

typedef struct PreloadedAudio {
  WavReader *pickupCoin, *powerUp;
} PreloadedAudio;

typedef struct PreloadedModels {
  Wavefront* box;
} PreloadedModels;

typedef struct PreloadedTextures {
  BmpReader *atlas, *glyphs0;
} PreloadedTextures;

typedef struct Logic__State {
  // Menu* menu;
  Level* level;
  Player* player;
  u32 lastUid;
  PreloadedAudio audio;
  PreloadedModels models;
  PreloadedTextures textures;
  WavReader* aSrc;  // current audio source

  bool mouseCaptured;
  KbInputState* kbState;
  PointerInputState* mState;

  sg_pass_action* pass_action;

} Logic__State;

#undef MAX_LISTENERS