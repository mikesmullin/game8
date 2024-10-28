#include "Dispatcher.h"

#include "../entities/CatEntity.h"
#include "../entities/DebugText.h"
#include "../entities/Player.h"
#include "../entities/RubbleSprite.h"
#include "../entities/Sprite.h"
#include "../entities/blocks/BreakBlock.h"
#include "../entities/blocks/CatSpawnBlock.h"
#include "../entities/blocks/RedWallBlock.h"
#include "../entities/blocks/SpawnBlock.h"
#include "../entities/blocks/WallBlock.h"

// #include "../menus/AboutMenu.h"
// #include "../menus/HelpMenu.h"
// #include "../menus/TitleMenu.h"
#include "../Logic.h"

extern Engine__State* g_engine;

static void Dispatch__None1(Entity* inst) {
}
static void Dispatch__None2(Entity* inst, void* params) {
}

static void (*VTABLE_ENGINE1[])(Entity*) = {
    Dispatch__None1,

    BreakBlock__render,
    CatEntity__gui,
    CatEntity__render,
    CatEntity__tick,
    CatSpawnBlock__gui,
    CatSpawnBlock__tick,
    DebugText__tick,
    Player__tick,
    RubbleSprite__render,
    RubbleSprite__tick,
    SpawnBlock__tick,
    Sprite__render,
    WallBlock__render,

    // AboutMenu__gui,
    // AboutMenu__render,
    // AboutMenu__tick,
    // HelpMenu__gui,
    // HelpMenu__render,
    // HelpMenu__tick,
    // TitleMenu__gui,
    // TitleMenu__render,
    // TitleMenu__tick,

    Dispatch__None1,  // LEVEL__TICK
    Dispatch__None1,  // LEVEL__TICK__ARENA_RESET,
    Dispatch__None1,  // LEVEL__TICK__QUADTREE_CREATE,

};

static void (*VTABLE_ENGINE2[])(Entity*, void*) = {
    Dispatch__None2,

    BreakBlock__action,
    CatEntity__action,
    CatEntity__collide,
    RedWallBlock__action,
};

// static / switch / tag / conditional dispatch
// a hot-reload safe, simple alternative to polymorphism
// works because all fns are known at compile-time
void Dispatcher__call1(DispatchFnId1 id, Entity* inst) {
  VTABLE_ENGINE1[id](inst);
}
void Dispatcher__call2(DispatchFnId2 id, Entity* inst, void* params) {
  VTABLE_ENGINE2[id](inst, params);
}