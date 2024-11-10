#include "../../../../src/game/common/Base64.h"

#include <string.h>

#include "../../../../src/game/common/Arena.h"
#include "../../../../src/lib/Log.h"

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