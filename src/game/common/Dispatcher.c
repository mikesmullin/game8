#include "Dispatcher.h"

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

static void Dispatch__None() {
}

static void (*VTABLE_ENGINE[])() = {
    Dispatch__None,

    // CatSpawnBlock__tick,  //
    // CatSpawnBlock__gui,
    SpawnBlock__tick,  //
    WallBlock__render,  //
    // BreakBlock__render,  //
    // BreakBlock__tick,

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

// static / switch / tag / conditional dispatch
// a hot-reload safe, simple alternative to polymorphism
// works because all fns are known at compile-time
void Dispatcher__call1(DispatchFnId id, void* inst) {
  return VTABLE_ENGINE[id](inst);
}
void Dispatcher__call2(DispatchFnId id, void* inst, void* params) {
  return VTABLE_ENGINE[id](inst, params);
}