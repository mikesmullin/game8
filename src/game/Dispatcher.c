#include "Dispatcher.h"

void CatEntity__tick(void* params);
void CatSpawnBlock__tick(void* params);
void DebugText__tick(void* params);
void Player__tick(void* params);
void RubbleSprite__tick(void* params);
void SkyBox__tick(void* params);
void SpawnBlock__tick(void* params);

void BreakBlock__action(void* params);
void CatEntity__action(void* params);
void CatEntity__collide(void* params);
void RedWallBlock__action(void* params);

void Atlas__onrender_load(void* params);
void Atlas__onrender_alloc(void* params);
void Atlas__onrender_entity(void* params);
void Atlas__onrender_material(void* params);
void PBR__onrender_load(void* params);
void PBR__onrender_alloc(void* params);
void PBR__onrender_entity(void* params);
void PBR__onrender_material(void* params);

static void Dispatch__None(void* params) {
}

static void (*VTABLE_ENGINE[])(void*) = {
    Dispatch__None,

    // Entity*
    CatEntity__tick,
    CatSpawnBlock__tick,
    DebugText__tick,
    Player__tick,
    RubbleSprite__tick,
    SkyBox__tick,
    SpawnBlock__tick,

    // AboutMenu__gui,
    // AboutMenu__render,
    // AboutMenu__tick,
    // HelpMenu__gui,
    // HelpMenu__render,
    // HelpMenu__tick,
    // TitleMenu__gui,
    // TitleMenu__render,
    // TitleMenu__tick,

    Dispatch__None,  // LEVEL__TICK
    Dispatch__None,  // LEVEL__TICK__ARENA_RESET,
    Dispatch__None,  // LEVEL__TICK__QUADTREE_CREATE,
    Dispatch__None,  // LEVEL__RENDER
    Dispatch__None,  // LEVEL__GUI
    Dispatch__None,  // GAME__GUI

    // Entity*, void*
    BreakBlock__action,
    CatEntity__action,
    CatEntity__collide,
    RedWallBlock__action,

    // void*
    Atlas__onrender_load,
    Atlas__onrender_alloc,
    Atlas__onrender_entity,
    Atlas__onrender_material,
    PBR__onrender_load,
    PBR__onrender_alloc,
    PBR__onrender_entity,
    PBR__onrender_material,
};

// static / switch / tag / conditional dispatch
// a hot-reload safe, simple alternative to polymorphism
// works because all fns are known at compile-time
void Dispatcher__call(u32 id, void* params) {
  VTABLE_ENGINE[id](params);
}
