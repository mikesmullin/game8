#define ENGINE__NO_MAIN
#include "../../../../src/engine/Engine.h"  // IWYU pragma: keep

//

#include "../../../../src/engine/common/Date.h"

// @describe Date
// @tag common
int main() {
  u64 epoch = Date__now();
  ASSERT(epoch > 0);

  return 0;
}