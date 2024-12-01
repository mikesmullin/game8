#include "BreakBlock.h"

void BreakBlock__init(Entity* entity, f32 x, f32 y) {
  Block* block = (Block*)entity;
  BreakBlock* self = (BreakBlock*)block;
  WallBlock__init(entity, x, y);
  entity->dispatch->action = BREAK_BLOCK__ACTION;
  entity->tags1 |= TAG_BRICK;
  entity->tags1 |= TAG_USEABLE;

  self->sg = Arena__Push(g_engine->arena, sizeof(StateGraph));
  self->sg->currentState = 0;
  self->sg->entity = entity;

  entity->render = Arena__Push(g_engine->arena, sizeof(RendererComponent));

  // preload assets
  entity->render->material = Preload__material(&g_engine->materials->wall, sizeof(Material));
  entity->render->material->mesh = Preload__model(  //
      &g_engine->models->box,
      "../assets/models/",
      "box.obj");
  entity->render->material->shader = g_engine->shaders->atlas;
  entity->render->material->texture0 = Preload__texture(  //
      &g_engine->textures->atlas,
      "../assets/textures/atlas.bmp");
  entity->render->ti = g_engine->game->level->wallTex + 1;
  entity->render->tw = entity->render->th = 8;
  entity->render->aw = entity->render->ah = 64;
  entity->render->color = g_engine->game->level->wallCol;

  Preload__audio(
      &g_engine->audio->bash,  //
      "../assets/audio/sfx/bash.wav");
}

void BreakBlock__action(void* _params) {
  OnActionParams* action = _params;
  Entity* entity = (Entity*)action->target;
  BreakBlock* self = (BreakBlock*)entity;
  Player* player1 = (Player*)g_engine->players->head->data;

  if (ACTION_USE == action->type) {
    if (!(TAG_BROKEN & entity->tags1)) {
      entity->tags1 |= TAG_BROKEN;
      entity->removed = true;
      AudioSource__play(g_engine->audio->bash, action->target, (Entity*)action->actor);

      for (u32 i = 0; i < 32; i++) {
        RubbleSprite* rs = Arena__Push(g_engine->arena, sizeof(RubbleSprite));
        RubbleSprite__init((Entity*)rs);
        rs->base.base.tform->pos.x = entity->tform->pos.x;
        rs->base.base.tform->pos.y = entity->tform->pos.y;
        rs->base.base.tform->pos.z = entity->tform->pos.z;
        List__append(g_engine->arena, g_engine->game->level->entities, rs);
      }
    }
  }
}
