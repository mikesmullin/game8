#pragma once

#define MAX_LISTENERS 10

typedef enum EventType {
  EVENT_NONE,
  EVENT_HELLO,
  EVENT_GOODBYE,
  EVENT_DEATH,
  EVENT_ATTACKED,
  EVENT_ANIMOVER,
} EventType;

typedef enum ListenerFnId {
  BLOCK__HELLO,
  ENTITY__HELLO,
  CAT_ENTITY__GOODBYE,
  LISTENER_FN_COUNT
} ListenerFnId;

typedef struct EventEmitter {
  EventType event[MAX_LISTENERS];
  ListenerFnId listeners[MAX_LISTENERS];
  int count;
} EventEmitter;

void EventEmitter__init(EventEmitter* emitter);
void EventEmitter__on(EventEmitter* emitter, EventType event, ListenerFnId listener);
void EventEmitter__emit(EventEmitter* emitter, EventType event, void* self, void* data);
