#define ENGINE__NO_MAIN
#include "../../../../src/engine/common/Sha1.h"

#include <string.h>

#include "../../../../src/engine/Engine.h"  // IWYU pragma: keep

// @describe Sha1
// @tag common
int main() {
  u8 hash[20];
  sha1(hash, (const u8*)&"tunafish", 8);

  char out[1024];
  hexdump(hash, 20, out, 1024);
  char* expected =
      "00000000  55 cb 62 40 74 52 a6 f7 55 f8 ff 7c 54 d4 af 4a  |U.b@tR..U..|T..J|\n"
      "00000010  db ea db 8c                                      |....            |\n";
  ASSERT(0 == strncmp(out, expected, strlen(expected)));

  return 0;
}