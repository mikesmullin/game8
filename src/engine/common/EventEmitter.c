#include "EventEmitter.h"

// Initialize the EventEmitter
void EventEmitter__init(EventEmitter* emitter) {
  emitter->count = 0;
}

// Register a listener for an event using ListenerFnId
void EventEmitter__on(EventEmitter* emitter, EventType event, ListenerFnId listener) {
  if (emitter->count < MAX_LISTENERS) {
    emitter->event[emitter->count] = event;
    emitter->listeners[emitter->count] = listener;
    emitter->count++;
  }
}

// Emit an event using the listener enum dispatch
// TODO: pass vtable here
void EventEmitter__emit(EventEmitter* emitter, EventType event, void* self, void* data) {
  for (int i = 0; i < emitter->count; i++) {
    EventType e = emitter->event[i];
    if (e == event) {
      ListenerFnId listener = emitter->listeners[i];
      // VTABLE_LISTENERS[listener](self, data);  // Dispatch event to correct listener
    }
  }
}