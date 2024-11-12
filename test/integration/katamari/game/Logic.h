#pragma once

#include "../../../../src/engine/Engine.h"

// Globals ---------------------------------------------

#define WINDOW_SIZE (640)
#define SCREEN_SIZE ((u32)(WINDOW_SIZE / 4))

// Engine ----------------------------------------------

typedef struct Arena Arena;
typedef struct Socket Socket;
typedef struct WavReader WavReader;
typedef struct Wavefront Wavefront;
typedef struct BmpReader BmpReader;
typedef struct List List;
typedef struct Entity Entity;
typedef struct BTNode_t BTNode;

typedef struct Logic__State Logic__State;
typedef struct QuadTreeNode QuadTreeNode;
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
  u32 mpTexture;  // resource id for gpu texture (ie. multi-pass rendering)
  sg_pipeline* pipe;
  sg_bindings* bind;
  bool loaded;
} Material;

typedef enum RenderGroup {
  WORLD_UNSORT_RG,  // ie. 3D (default)
  WORLD_ZSORT_RG,  // ie. 3D transparent
  UI_ZSORT_RG,  // ie. 2D transparent
  SKY_RG,  // skybox
  SCREEN_RG,  // screen
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

typedef enum EntityTags1 /* : u64 */ {
  TAG_NONE = 0,
  TAG_LOADED = 1 << 1,
  TAG_WALL = 1 << 2,
  TAG_CAT = 1 << 3,
  TAG_CATSPAWN = 1 << 4,
  TAG_BRICK = 1 << 5,
  TAG_BROKEN = 1 << 6,
  TAG_SKY = 1 << 7,
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

typedef enum SGStateTags1 /* : u64 */ {
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
  bool loaded;
  List* entities;  // world entities
  List* nzentities;
  List* zentities;  // zsort world entities (ie. transparent entities only)
  Entity* cubemap;
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
  Wavefront *box, *plane2D, *skybox, *screen2D;
} PreloadedModels;

typedef struct PreloadedTextures {
  BmpReader *atlas, *glyphs0, *sky;
} PreloadedTextures;

typedef struct PreloadedMaterials {
  Material *wall, *sprite, *glyph, *cubemap, *screen;
} PreloadedMaterials;

#define LISTEN_ADDR "0.0.0.0"
#define LISTEN_PORT "9000"
#define CONNECT_ADDR "127.0.0.1"
#define MAX_CLIENTS (1)

typedef struct NetMgr {
  Socket* listener;
  Socket* clients[MAX_CLIENTS];
  u32 client_count;
  Socket* client;
} NetMgr;

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

  bool mouseCaptured;
  KbInputState* kbState;
  PointerInputState* mState;

  sg_pass *pass1, *pass2;
  List* screen;
  DebugText* dt;
  NetMgr net;
} Logic__State;

HOT_RELOAD__EXPORT void logic_oninit(Engine__State* state);
HOT_RELOAD__EXPORT void logic_onpreload(void);
HOT_RELOAD__EXPORT void logic_onevent(const sapp_event* event);
HOT_RELOAD__EXPORT void logic_onfixedupdate(void);
HOT_RELOAD__EXPORT void logic_onupdate(void);
HOT_RELOAD__EXPORT void logic_onshutdown(void);

#undef MAX_LISTENERS