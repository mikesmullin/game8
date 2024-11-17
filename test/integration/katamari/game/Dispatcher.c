#include "Dispatcher.h"

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
