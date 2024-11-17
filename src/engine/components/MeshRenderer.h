#pragma once

#include "../Engine.h"  // IWYU pragma: keep

typedef void (*MeshRenderer__cb0)(u32 id, void* params);

void MeshRenderer__renderBatches(List* entities, MeshRenderer__cb0 cb);