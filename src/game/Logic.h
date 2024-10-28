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
typedef struct sapp_event sapp_event;
typedef struct sg_image_data sg_image_data;
typedef struct sg_frame_stats sg_frame_stats;

typedef struct Engine__State {
  char window_title[255];

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
  double (*stm_ns)(uint64_t ticks);
  double (*stm_us)(uint64_t ticks);
  double (*stm_ms)(uint64_t ticks);
  double (*stm_sec)(uint64_t ticks);
  uint64_t (*stm_laptime)(uint64_t* last_time);
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
  void (*sg_update_buffer)(sg_buffer buf_id, const sg_range* data);
  void (*sg_update_image)(sg_image img_id, const sg_image_data* data);
  sg_frame_stats (*sg_query_frame_stats)(void);
  void (*sg_enable_frame_stats)(void);
  void (*sg_disable_frame_stats)(void);
  bool (*sg_frame_stats_enabled)(void);
  void (*sapp_lock_mouse)(bool lock);
  bool (*sapp_mouse_locked)(void);

  u64 now;
  f32 deltaTime;
  u16 entity_count;
  u16 draw_count;

} Engine__State;

#ifdef __EMSCRIPTEN__
void logic_oninit(Engine__State* state);
void logic_onpreload(void);
void logic_onreload(Engine__State* state);
void logic_onevent(const sapp_event* event);
void logic_onfixedupdate(void);
void logic_onupdate(void);
void logic_onshutdown(void);
#else
typedef void (*logic_oninit_t)(Engine__State* state);
typedef void (*logic_onpreload_t)(void);
typedef void (*logic_onreload_t)(Engine__State* state);
typedef void (*logic_onevent_t)(const sapp_event* event);
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
typedef struct BTNode_t BTNode;

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
  u32 /*DispatchFnId2*/ action;
} EngineComponent;

typedef struct TransformComponent {
  v3 pos;  // (x, y, z)
  v3 rot;  // (yaw, pitch, roll)
  v3 scale;  // (sx, sy, sz)
} TransformComponent;

typedef enum ColliderType {
  BOX_COLLIDER_2D,
  CIRCLE_COLLIDER_2D,
} ColliderType;

typedef struct OnCollideParams {
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

// TODO: kindof misnomer; contains mesh + texture + shader + bindings
typedef struct Material {
  Wavefront* mesh;
  BmpReader* texture;
  sg_pipeline* pipe;
  sg_bindings* bind;
  bool loaded;
} Material;

typedef enum RenderGroup {
  WORLD_UNSORT_RG,  // ie. 3D (default)
  WORLD_ZSORT_RG,  // ie. 3D transparent
  UI_ZSORT_RG,  // ie. 2D transparent
} RenderGroup;

typedef struct RendererComponent {
  RenderGroup rg;
  Material* material;
  u32 ti, pi, po,  // texture index, palette index, palette offset
      tw, th,  // texture width x  height
      aw, ah;  // atlas width x height
  bool useMask, billboard, indexedPalette;
  u32 mask, color;
} RendererComponent;

typedef struct HealthComponent_t {
  f32 hp;
  f32 hurtTime;
} HealthComponent;

typedef struct EventEmitterComponent_t {
} EventEmitterComponent;

typedef struct AudioListenerComponent {
} AudioListenerComponent;

typedef struct AudioSourceComponent {
} AudioSourceComponent;

// Entities ----------------------------------------------

typedef enum EntityTags1 : u64 {
  TAG_NONE = 0,
  TAG_LOADED = 1 << 1,
  TAG_WALL = 1 << 2,
  TAG_CAT = 1 << 3,
  TAG_CATSPAWN = 1 << 4,
  TAG_BRICK = 1 << 5,
  TAG_BROKEN = 1 << 6,
} EntityTags1;

typedef struct Entity {
  u32 id;
  u64 tags1;
  bool removed;
  EngineComponent* engine;
  TransformComponent* tform;
  EventEmitterComponent* event;
  ColliderComponent* collider;
  Rigidbody2DComponent* rb;
  RendererComponent* render;
  HealthComponent* health;
  // // TODO: if we don't need to iterate, these can be moved within subclass
  AudioSourceComponent* audio;
  AudioListenerComponent* hear;
  EventEmitter* events;
} Entity;

typedef enum ProjectionType {
  PERSPECTIVE_PROJECTION,
  ORTHOGRAPHIC_PROJECTION,
} ProjectionType;

typedef struct Projection {
  ProjectionType type;
  f32 fov;  // field of view
  f32 nearZ;  // near plane
  f32 farZ;  // far plane
} Projection;

typedef struct VirtualJoystick {
  f32 xAxis, yAxis, zAxis;
} VirtualJoystick;

typedef struct PointerState {
  s32 x, y;
  f32 wheely;
} PointerState;

#define KEYCODE_W 87
#define KEYCODE_A 65
#define KEYCODE_S 83
#define KEYCODE_D 68
#define KEYCODE_E 69
#define KEYCODE_Q 81
#define KEYCODE_TAB 258
#define KEYCODE_SPACE 32
#define KEYCODE_LCTRL 341
#define KEYCODE_R 82
#define KEYCODE_ESC 256

typedef struct InputState {
  bool fwd;
  bool back;
  bool left;
  bool right;
  bool use;
  bool up;
  bool down;
  bool reload;
  bool esc;
} InputState;

typedef struct Player {
  Entity base;
  Projection proj;
  f32 bobPhase;
  u32 lastInput;
  VirtualJoystick joy;
  PointerState ptr;
  InputState input;
} Player;

typedef enum ActionType {
  ACTION_NONE,  //
  ACTION_USE,
} ActionType;

typedef struct Action {
  ActionType type;
  Entity* actor;
  Entity* target;
} Action;

typedef enum SGStateTags1 : u64 {
  SGST_NONE = 0,  //
  SGST_BUSY = 1 << 1,
} SGStateTags1;

typedef struct StateGraph StateGraph;
typedef struct SGState SGState;
typedef void (*SGStateFn)(StateGraph* sg);
typedef void (*SGActionFn)(StateGraph* sg, Action* action);

typedef struct SGStateKeyframe {
  u32 id;
  SGStateFn cb;
} SGStateKeyframe;

typedef struct SGState {
  SGStateFn onEnter;
  SGStateFn onUpdate;
  SGStateFn onExit;
  u32 frameCount;
  u32 keyframeCount;
  SGStateKeyframe* keyframes;
  // eventListeners[];
} SGState;

typedef enum SGFSM_t {
  SGFSM_NULL,  //
  SGFSM_ENTERING,
  SGFSM_UPDATING,
  SGFSM_EXITING,
} SGFSM;

typedef struct StateGraph {
  Entity* entity;
  u32 currentState;
  SGFSM fsm;
  u32 frame;
  // SGActionFn actions;
  EventEmitter events;
  u64 tags1;  // SGStateTags1
} StateGraph;

typedef struct Sprite {
  Entity base;
} Sprite;

typedef struct DebugText {
  Entity base;
  char* txt;
  List* glyphs;
} DebugText;

typedef struct Block {
  Entity base;
  bool masked;
} Block;

typedef struct WallBlock {
  Block base;
} WallBlock;

typedef struct RubbleSprite {
  Sprite base;
  f32 xa, ya, za;
  f32 life, lifeSpan;
} RubbleSprite;

typedef struct BreakBlock {
  Block base;
  StateGraph* sg;
} BreakBlock;

typedef struct SpawnBlock {
  Block base;
  bool firstTick;
} SpawnBlock;

typedef struct CatSpawnBlock {
  Block base;
  u32 spawnCount;
  f32 spawnInterval;
  f32 animTime;
  u32 spawnedCount;
  u32 maxSpawnCount;
} CatSpawnBlock;

typedef struct CatEntity {
  Sprite base;
  f32 xa, ya, za;
  StateGraph* sg;
  BTNode* brain;
} CatEntity;

// Game ----------------------------------------------

typedef struct Level {
  BmpReader* bmp;
  char* levelFile;
  bool skybox;
  BmpReader* world;
  char* worldFile;
  bool loaded;
  List* entities;  // world entities
  List* zentities;  // zsort world entities (ie. transparent entities only)
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
  QuadTreeNode* qt;
} Level;

typedef struct PreloadedAudio {
  WavReader *pickupCoin, *powerUp, *bash, *meow, *click;
} PreloadedAudio;

typedef struct PreloadedModels {
  Wavefront *box, *plane2D;
} PreloadedModels;

typedef struct PreloadedTextures {
  BmpReader *atlas, *glyphs0;
} PreloadedTextures;

typedef struct PreloadedMaterials {
  Material *wall, *sprite, *glyph;
} PreloadedMaterials;

typedef struct Logic__State {
  // Menu* menu;
  Level* level;
  Player* player;
  Player* camera;
  List* ui_entities;
  u32 lastUid;
  PreloadedAudio audio;
  PreloadedModels models;
  PreloadedTextures textures;
  PreloadedMaterials materials;
  WavReader* aSrc;  // current audio source

  bool mouseCaptured;
  KbInputState* kbState;
  PointerInputState* mState;

  sg_pass_action* pass_action;

  // TODO: there should be an arena per game, frame, etc.
  Arena* frameArena;
  DebugText* dt;

} Logic__State;

#undef MAX_LISTENERS