#define ENGINE__NO_MAIN
#include "../../../../src/engine/Engine.h"  // IWYU pragma: keep

//

#include "../../../../src/engine/common/Hash.h"
#include "../../../../src/engine/common/Math.h"

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

  // LOG_DEBUGF("HashTable__set");
  HashTable__set(arena, t, "Fn1", (void*)Fn1);
  HashTable__set(arena, t, "PATH", (void*)PATH);
  HashTable__set(arena, t, "x", (void*)&x);
  HashTable__set(arena, t, "X_a", &X_a);
  HashTable__set(arena, t, "X_b", &X_b);

  const HashTable_Node* v;

  // LOG_DEBUGF("HashTable__get");

  ASSERT(193456906 == Hash__djb2("Fn1", 3));
  ASSERT(2103559796 == Hash__fmix32(Hash__djb2("Fn1", 3)));
  ASSERT(116 == Math__scaleu(0, Hash__fmix32(Hash__djb2("Fn1", 3)), 255));

  ASSERT(2089422194 == Hash__djb2("PATH", 4));
  ASSERT(2228291689 == Hash__fmix32(Hash__djb2("PATH", 4)));
  ASSERT(105 == Math__scaleu(0, Hash__fmix32(Hash__djb2("PATH", 4)), 255));

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