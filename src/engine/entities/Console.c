#include "Console.h"

#include "../Engine.h"
#include "../common/Script.h"

static const u32 MAX_TOKENS = 255;
static const char PROMPT = '>';
static const char CURSOR = 0x7f;  // block cursor glyph

void Console__init(Entity* entity) {
  Console* self = (Console*)entity;
  Entity__init(entity);

  self->show = false;
  self->buf[0] = '\0';

  self->vtable = HashTable__alloc(g_engine->arena);
}

static void Console__exec(Entity* entity) {
  Console* self = (Console*)entity;

  Script__Token tokens[MAX_TOKENS];
  size_t token_count = Script__tokenize(self->buf, tokens, MAX_TOKENS);
  // Script__printTokens(tokens, token_count);

  List* stack = List__alloc(g_engine->frameArena);
  Script__exec(g_engine->frameArena, tokens, token_count, self->vtable, stack);
}

bool Console__event(Entity* entity, const sapp_event* event) {
  Console* self = (Console*)entity;

  if (!self->show) {
    if (SAPP_EVENTTYPE_CHAR == event->type) {
      if ('~' == event->char_code) {
        self->show = true;
        // return true;
      }
    }
  } else if (self->show) {
    if (SAPP_EVENTTYPE_KEY_DOWN == event->type) {
      if (KEYCODE_ESC == event->key_code) {
        self->show = false;
        return true;
      }
      if (SAPP_KEYCODE_BACKSPACE == event->key_code) {
        if (self->len > 0) {
          self->len--;
          self->buf[self->len] = '\0';
        }
      } else if (SAPP_KEYCODE_DELETE == event->key_code) {
        // TODO: implement?
      } else if (SAPP_KEYCODE_ENTER == event->key_code) {
        Console__exec(entity);
        self->len = 0;
        self->buf[0] = '\0';
      } else {
        // LOG_DEBUGF("keydown code %u", event->key_code);
      }
    }
    if (SAPP_EVENTTYPE_CHAR == event->type) {
      // LOG_DEBUGF("char %u %c", event->char_code, event->char_code);
      self->buf[self->len++] = event->char_code;
      self->buf[self->len] = '\0';
    }
  }

  if (self->show) {
    if (SAPP_EVENTTYPE_KEY_DOWN == event->type || SAPP_EVENTTYPE_CHAR == event->type) {
      LOG_DEBUGF("> %s█", self->buf);
    }

    return true;
  }

  return false;
}

void Console__tick(Entity* entity) {
  Console* self = (Console*)entity;

  if (!self->show) {
    self->render[0] = '\0';
  } else {
    // TODO: fix mprintf() and use it here
    sprintf(self->render, "%c %s%c", PROMPT, self->buf, CURSOR);
  }
}
