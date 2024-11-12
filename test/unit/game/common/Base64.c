#define ENGINE__NO_MAIN
#include "../../../../src/engine/common/Base64.h"

#include "../../../../src/engine/Engine.h"  // IWYU pragma: keep

// @describe Base64
// @tag common
int main() {
  Arena* arena;
  Arena__Alloc(&arena, 1024);

  const char* clear = "tunafish";
  const char* expected = "dHVuYWZpc2g=";
  char* encoded = base64_encode(arena, (const unsigned char*)clear, 8);
  ASSERT(0 == strcmp(encoded, expected));

  return 0;
}