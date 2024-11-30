#pragma once

// Engine -----------------------------------------------

#include "../engine/Engine.h"

// Globals ----------------------------------------------

#define WINDOW_SIZE (640)
#define SCREEN_SIZE ((u32)(WINDOW_SIZE / 4))

typedef struct PreloadedAudio {
  WavReader *pickupCoin, *powerUp, *bash, *meow, *click;
} PreloadedAudio;

typedef struct PreloadedModels {
  Wavefront *box, *plane2D, *skybox, *screen2D;
} PreloadedModels;

typedef struct PreloadedTextures {
  BmpReader *atlas, *glyphs0, *sky;
} PreloadedTextures;

typedef struct PreloadedShaders {
  Shader *atlas, *pbr;
} PreloadedShaders;

typedef struct PreloadedMaterials {
  Material *wall, *sprite, *glyph, *cubemap, *screen;
} PreloadedMaterials;

typedef struct Level Level;
typedef struct SpawnBlock SpawnBlock;
typedef struct DebugText DebugText;
typedef struct NetMgr NetMgr;

typedef struct Game {
  // Menu* menu;
  Level* level;
  sg_pass *pass1, *pass2;
  List* ui_entities;
  List* screen;
  DebugText* dt;
  NetMgr* net;
} Game;

void Game__init();
void Game__reload();
void Game__preload();
void Game__tick();
void Game__render();
void Game__gui();
void Game__postprocessing();
void Game__shutdown();

// Convenience -------------------------------------------

typedef struct BTNode_t BTNode;
typedef struct Sprite Sprite;

#include "Dispatcher.h"  // IWYU pragma: keep
#include "entities/Sprite.h"  // IWYU pragma: keep
#include "entities/blocks/Block.h"  // IWYU pragma: keep
#include "levels/Level.h"  // IWYU pragma: keep
#include "messages/Message.h"  // IWYU pragma: keep

// Entities ----------------------------------------------

typedef struct Player Player;

#include "entities/CatEntity.h"  // IWYU pragma: keep
#include "entities/DebugText.h"  // IWYU pragma: keep
#include "entities/NetMgr.h"  // IWYU pragma: keep
#include "entities/Player.h"  // IWYU pragma: keep
#include "entities/RubbleSprite.h"  // IWYU pragma: keep
#include "entities/Screen.h"  // IWYU pragma: keep
#include "entities/SkyBox.h"  // IWYU pragma: keep
#include "entities/blocks/BreakBlock.h"  // IWYU pragma: keep
#include "entities/blocks/CatSpawnBlock.h"  // IWYU pragma: keep
#include "entities/blocks/RedWallBlock.h"  // IWYU pragma: keep
#include "entities/blocks/SpawnBlock.h"  // IWYU pragma: keep
#include "entities/blocks/WallBlock.h"  // IWYU pragma: keep
