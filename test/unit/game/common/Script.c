#define ENGINE__NO_MAIN
#include "../../../../src/engine/Engine.h"  // IWYU pragma: keep

//

#include "../../../../src/engine/common/Arena.h"
#include "../../../../src/engine/common/Hash.h"
#include "../../../../src/engine/common/List.h"
#include "../../../../src/engine/common/Math.h"
#include "../../../../src/engine/common/Script.h"

#define MAX_TOKENS 1024

static void getTagW(Arena* arena, const HashTable* vtable, List* stack) {
  ASSERT_CONTEXT(stack->len >= 1, "getTag() expects 1 parameter.");
  Script__Token* t = List__pop(stack);
  // lookup string => int
  HashTable_Node* node = HashTable__get(vtable, t->value);
  ASSERT_CONTEXT(NULL != node, "Undefined tag: %s", t->value);
  // return via stack
  List__append(arena, stack, (void*)(u64)node->value);
}

static Entity fakeEntity = {.id = 6, .tags1 = TAG_CATSPAWN};

static void Level__findEntityW(Arena* arena, const HashTable* vtable, List* stack) {
  ASSERT_CONTEXT(stack->len >= 1, "Level__findEntity() expects 1 parameter.");
  u32 tag = (u64)List__pop(stack);
  // return via stack
  List__append(arena, stack, &fakeEntity);
}

static void setW(Arena* arena, const HashTable* vtable, List* stack) {
  ASSERT_CONTEXT(stack->len >= 3, "set() expects 3 parameters.");
  Entity* o = List__pop(stack);
  ASSERT_CONTEXT(NULL != o, "Expected Entity type as parameter 1. got: NULL");

  Script__Token* k = List__pop(stack);
  ASSERT_CONTEXT(TOKEN_WORD == k->type, "Expected WORD type as parameter 2. got: %u", k->type);
  HashTable_Node* node = HashTable__get(vtable, k->value);
  ASSERT_CONTEXT(NULL != node, "Undefined Entity property: %s", k->value);
  u32 offset = (u64)node->value;

  Script__Token* v = List__pop(stack);
  ASSERT_CONTEXT(TOKEN_U32 == v->type, "Expected U32 type as parameter 3. got: %u", v->type);
  u32 value;
  msscanf(v->value, "%du", &value);

  LOG_DEBUGF("would setEntityU %p offset: %u, value: %u", o, offset, value);
}

typedef struct MockCatSpawnBlock {
  u32 a, maxSpawnCount;
} MockCatSpawnBlock;

// @describe Script
// @tag common
int main() {
  const char* source_code;
  // source_code = "set (Level__findEntity (getTag TAG_CATSPAWN)) maxSpawnCount 1u";
  // source_code = "set(Level__findEntity(getTag TAG_CATSPAWN maxSpawnCount 1u";
  source_code = "set (Level__findEntity(getTag TAG_CATSPAWN)), maxSpawnCount, 1u";

  Script__Token tokens[MAX_TOKENS];
  size_t token_count = Script__tokenize(source_code, tokens, MAX_TOKENS);
  Script__printTokens(tokens, token_count);

  Arena* arena = Arena__Alloc(1024);
  List* stack = List__alloc(arena);
  HashTable* vtable = HashTable__alloc(arena);
  HashTable__set(arena, vtable, "set", &setW);
  HashTable__set(arena, vtable, "Level__findEntity", &Level__findEntityW);
  HashTable__set(arena, vtable, "getTag", &getTagW);
  HashTable__set(arena, vtable, "TAG_CATSPAWN", (void*)TAG_CATSPAWN);
  HashTable__set(
      arena,
      vtable,
      "maxSpawnCount",
      (void*)(u64)offsetof(MockCatSpawnBlock, maxSpawnCount));

  Script__exec(arena, tokens, token_count, vtable, stack);

  return 0;
}