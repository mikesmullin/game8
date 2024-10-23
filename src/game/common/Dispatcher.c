#include "Dispatcher.h"

#include "../entities/Sprite.h"
// #include "../blocks/Block.h"
// #include "../blocks/BreakBlock.h"
// #include "../blocks/CatSpawnBlock.h"
#include "../entities/blocks/SpawnBlock.h"
#include "../entities/blocks/WallBlock.h"
// #include "../entities/CatEntity.h"
// #include "../entities/Entity.h"
#include "../entities/Player.h"
// #include "../menus/AboutMenu.h"
// #include "../menus/HelpMenu.h"
// #include "../menus/TitleMenu.h"
// #include "../sprites/Sprite.h"
#include "../Logic.h"

extern Engine__State* g_engine;

static void Dispatch__None1(Entity* inst) {
}
static void Dispatch__None2(Entity* inst, void* params) {
}

static void (*VTABLE_ENGINE1[])(Entity*) = {
    Dispatch__None1,

    // CatSpawnBlock__tick,  //
    // CatSpawnBlock__gui,
    SpawnBlock__tick,  //
    WallBlock__render,  //
    // BreakBlock__render,  //
    // BreakBlock__tick,

    Sprite__render,
    // CatEntity__tick,  //
    // CatEntity__render,  //
    // CatEntity__gui,  //
    // CatEntity__collide,  //
    Player__tick,  //

    // AboutMenu__tick,  //
    // AboutMenu__render,  //
    // AboutMenu__gui,  //
    // HelpMenu__tick,  //
    // HelpMenu__render,  //
    // HelpMenu__gui,  //
    // TitleMenu__tick,  //
    // TitleMenu__render,  //
    // TitleMenu__gui,  //
};

static void (*VTABLE_ENGINE2[])(Entity*, void*) = {
    Dispatch__None2,
};

// static / switch / tag / conditional dispatch
// a hot-reload safe, simple alternative to polymorphism
// works because all fns are known at compile-time
void Dispatcher__call1(DispatchFnId id, Entity* inst) {
  VTABLE_ENGINE1[id](inst);
}
void Dispatcher__call2(DispatchFnId id, Entity* inst, void* params) {
  VTABLE_ENGINE2[id](inst, params);
}