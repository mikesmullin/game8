#define ENGINE__NO_MAIN
#include "../../../../src/engine/Engine.h"  // IWYU pragma: keep

//

#include "../../../../src/engine/common/Hash.h"

typedef struct X {
  u32 a;
  u32 b;
} X;

void fn() {
}

static void* Fn1 = fn;
static char* PATH = "hello";
static u32 X_a = offsetof(X, a);
static u32 X_b = offsetof(X, b);

// @describe Hash
// @tag common
int main() {
  X x = (X){.a = 1, .b = 2};
  u8* p = (void*)&x;

  Arena* arena = Arena__Alloc(1024);
  HashTable* t = HashTable__alloc(arena);
  HashTable_Node n1 = {.key = "Fn1", .value = (void*)Fn1};
  HashTable_Node n2 = {.key = "PATH", .value = (void*)PATH};
  HashTable_Node n3 = {.key = "x", .value = (void*)&x};
  HashTable_Node n4 = {.key = "X_a", .value = &X_a};
  HashTable_Node n5 = {.key = "X_b", .value = &X_b};

  // LOG_DEBUGF("HashTable__set");
  HashTable__set(arena, t, &n1);
  HashTable__set(arena, t, &n2);
  HashTable__set(arena, t, &n3);
  HashTable__set(arena, t, &n4);
  HashTable__set(arena, t, &n5);

  const HashTable_Node* v;

  // LOG_DEBUGF("HashTable__get");

  v = HashTable__get(t, "Fn1");
  ASSERT(NULL != v);
  ASSERT(Fn1 == v->value);
  // LOG_DEBUGF("Fn %p %p", Fn1, v->value);

  v = HashTable__get(t, "PATH");
  ASSERT(NULL != v);
  ASSERT(PATH == v->value);
  // LOG_DEBUGF("PATH %s %s", PATH, v->value);

  v = HashTable__get(t, "x");
  ASSERT(NULL != v);
  ASSERT(&x == v->value);
  // LOG_DEBUGF("x %p %p", &x, v->value);

  v = HashTable__get(t, "X_a");
  ASSERT(NULL != v);
  ASSERT(&X_a == v->value);
  // LOG_DEBUGF("X_a %p %p", &X_a, v->value);

  v = HashTable__get(t, "X_b");
  ASSERT(NULL != v);
  ASSERT(&X_b == v->value);
  // LOG_DEBUGF("X_b %p %p", &X_b, v->value);

  return 0;
}