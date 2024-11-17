#include "DebugText.h"

void DebugText__init(Entity* entity, f32 x, f32 y, u32 len, char* txt, u32 color) {
  DebugText* self = (DebugText*)entity;
  Entity__init(entity);
  entity->dispatch->tick = DEBUG_TEXT__TICK;
  entity->tform->pos.x = x;
  entity->tform->pos.y = y;
  entity->tform->pos.z = 0;
  entity->tform->scale.x = entity->tform->scale.y = 20;
  self->txt = txt;
  self->glyphs = List__alloc(g_engine->arena);
  u32 slen = strlen(txt);
  for (u32 i = 0; i < len; i++) {
    Sprite* sprite = Arena__Push(g_engine->arena, sizeof(Sprite));
    Sprite__init((Entity*)sprite, 0, 0);
    sprite->base.tform->pos.x = x;
    sprite->base.tform->pos.y = y;
    sprite->base.tform->pos.z = 0;
    sprite->base.render->billboard = false;

    // preload assets
    sprite->base.render->material =
        Preload__material(&g_engine->materials->glyph, sizeof(Material));
    sprite->base.render->material->mesh = Preload__model(  //
        &g_engine->models->plane2D,
        "../assets/models/",
        "plane2D.obj");
    sprite->base.render->material->shader = g_engine->shaders->atlas;
    sprite->base.render->material->texture0 = Preload__texture(  //
        &g_engine->textures->glyphs0,
        "../assets/textures/glyphs0.bmp");
    sprite->base.render->ti = i < slen ? txt[i] : ' ';
    sprite->base.render->tw = 4, sprite->base.render->th = 6;  // glyph 4x6 pixels
    sprite->base.render->aw = 128, sprite->base.render->ah = 24;  // atlas 32x4 chars
    sprite->base.render->useMask = true;
    sprite->base.render->mask = COLOR_WHITE;
    sprite->base.render->color = color;
    sprite->base.render->rg = UI_ZSORT_RG;

    sprite->base.tform->pos.x = i * 4 * 4;

    sprite->base.tform->scale.x = (4.0f / 6) * entity->tform->scale.x;
    sprite->base.tform->scale.y = (6.0f / 6) * entity->tform->scale.y;
    sprite->base.tform->scale.z = 1;

    List__append(g_engine->arena, self->glyphs, sprite);
    List__insort(g_engine->arena, g_engine->game->ui_entities, sprite, Level__zsort);
  }
}

void DebugText__tick(Entity* entity) {
  PROFILE__BEGIN(DEBUG_TEXT__TICK);

  DebugText* self = (DebugText*)entity;

  u32 slen = strlen(self->txt);
  List__Node* c = self->glyphs->head;
  for (u32 i = 0; i < self->glyphs->len; i++) {
    Sprite* sprite = c->data;
    c = c->next;

    static f32 kerning = (f32)SCREEN_SIZE / (60 * 4);
    //mwave(1000, 0.60f, 0.63f);
    sprite->base.tform->pos.x = (entity->tform->pos.x) + (i * (entity->tform->scale.x * kerning));
    sprite->base.tform->pos.y = entity->tform->pos.y;
    sprite->base.tform->scale.x = (4.0f / 6) * entity->tform->scale.x;
    sprite->base.tform->scale.y = (6.0f / 6) * entity->tform->scale.y;
    sprite->base.render->ti = i < slen ? self->txt[i] : ' ';
  }
  PROFILE__END(DEBUG_TEXT__TICK);
}