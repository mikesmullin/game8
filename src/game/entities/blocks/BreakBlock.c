#include "BreakBlock.h"

#include "../../Logic.h"
#include "../../common/Arena.h"
#include "../../common/Dispatcher.h"
#include "../../common/List.h"
#include "../../common/Log.h"
#include "../../common/Preloader.h"
#include "../../components/AudioSource.h"
#include "../../components/MeshRenderer.h"
#include "../../levels/Level.h"
#include "../RubbleSprite.h"
#include "Block.h"

extern Engine__State* g_engine;

void BreakBlock__init(Entity* entity, f32 x, f32 y) {
  Logic__State* logic = g_engine->logic;
  Block* block = (Block*)entity;
  BreakBlock* self = (BreakBlock*)block;
  Block__init(block, x, y);
  entity->engine->render = BREAK_BLOCK__RENDER;
  entity->engine->tick = BREAK_BLOCK__TICK;
  entity->engine->action = BREAK_BLOCK__ACTION;
  entity->tags1 |= TAG_BRICK;

  self->sg = Arena__Push(g_engine->arena, sizeof(StateGraph));
  self->sg->currentState = 0;
  self->sg->entity = entity;

  entity->render = Arena__Push(g_engine->arena, sizeof(RendererComponent));

  // preload assets
  entity->render->material = Preload__material(&logic->materials.wall);
  entity->render->material->mesh = Preload__model(  //
      &logic->models.box,
      "../assets/models/box.obj");
  entity->render->material->texture = Preload__texture(  //
      &logic->textures.atlas,
      "../assets/textures/atlas.bmp");
  entity->render->ti = logic->level->wallTex + 1;
  entity->render->tw = entity->render->th = 8;
  entity->render->aw = entity->render->ah = 64;
  entity->render->color = logic->level->wallCol;

  Preload__audio(
      &logic->audio.bash,  //
      "../assets/audio/sfx/bash.wav");
}

void BreakBlock__render(Entity* entity) {
  Logic__State* logic = g_engine->logic;
  Block* block = (Block*)entity;
  BreakBlock* self = (BreakBlock*)block;

  if (TAG_BROKEN & entity->tags1) {
  } else {
    MeshRenderer__render(entity);
  }
}

void BreakBlock__tick(Entity* entity) {
  Logic__State* logic = g_engine->logic;
  Block* block = (Block*)entity;
  BreakBlock* self = (BreakBlock*)block;
}

void BreakBlock__action(Entity* entity, void* _action) {
  Logic__State* logic = g_engine->logic;
  BreakBlock* self = (BreakBlock*)entity;
  Action* action = (Action*)_action;

  if (ACTION_USE == action->type) {
    if (!(TAG_BROKEN & entity->tags1)) {
      entity->tags1 |= TAG_BROKEN;
      AudioSource__play(entity, logic->audio.bash);

      for (u32 i = 0; i < 32; i++) {
        RubbleSprite* rs = Arena__Push(g_engine->arena, sizeof(RubbleSprite));
        RubbleSprite__init((Entity*)rs);
        rs->base.base.tform->pos.x = entity->tform->pos.x;
        rs->base.base.tform->pos.y = entity->tform->pos.y;
        rs->base.base.tform->pos.z = entity->tform->pos.z;
        List__append(g_engine->arena, logic->level->entities, rs);
      }
    }
  }
}
