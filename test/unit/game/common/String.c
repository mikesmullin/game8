#define ENGINE__NO_MAIN
#include "../../../../src/engine/Engine.h"  // IWYU pragma: keep

//

#include "../../../../src/engine/common/Arena.h"
#include "../../../../src/engine/common/String.h"

// @describe String
// @tag common
int main() {
  Arena* arena = Arena__Alloc(1024);
  String8* s;
  s = String8__alloc(arena, 3);
  ASSERT(3 == s->sz);

  char cat[4] = "cat";
  s = String8__cstr(arena, cat);
  ASSERT(3 == s->sz);
  ASSERT('c' == s->str[0]);
  ASSERT('a' == s->str[1]);
  ASSERT('t' == s->str[2]);
  ASSERT(0 == s->str[3]);

  String8* s2 = String8__cp(arena, s);
  s->str[0] = 'b';
  ASSERT(3 == s2->sz);
  ASSERT('c' == s2->str[0]);
  ASSERT('a' == s2->str[1]);
  ASSERT('t' == s2->str[2]);
  ASSERT(0 == s2->str[3]);
  ASSERT('b' == s->str[0]);

  s = String8__cstr(arena, "conundrum");
  ASSERT(9 == s->sz);
  s2 = String8__slice(arena, s->str + 2, s->str + 2 + 3);
  ASSERT(3 == s2->sz);
  ASSERT('n' == s2->str[0]);
  ASSERT('u' == s2->str[1]);
  ASSERT('n' == s2->str[2]);
  ASSERT('d' == s2->str[3]);

  String8* s3 = String8__toUpper(arena, s2);
  ASSERT(3 == s3->sz);
  ASSERT('N' == s3->str[0]);
  ASSERT('U' == s3->str[1]);
  ASSERT('N' == s3->str[2]);
  ASSERT(0 == s3->str[3]);

  String8* s4 = String8__toLower(arena, s3);
  ASSERT(3 == s4->sz);
  ASSERT('n' == s4->str[0]);
  ASSERT('u' == s4->str[1]);
  ASSERT('n' == s4->str[2]);
  ASSERT(0 == s4->str[3]);

  s = String8__cstr(arena, "bob");
  s2 = String8__printf(arena, 255, "hi %s", s->str);
  ASSERT(6 == s2->sz);
  ASSERT('h' == s2->str[0]);
  ASSERT('i' == s2->str[1]);
  ASSERT(' ' == s2->str[2]);
  ASSERT('b' == s2->str[3]);
  ASSERT('o' == s2->str[4]);
  ASSERT('b' == s2->str[5]);
  ASSERT(0 == s2->str[6]);

  s3 = String8__cstr(arena, "sue");
  s4 = String8__cstr(arena, "bob");
  bool r;
  r = String8__cmp(s, s3);
  ASSERT(false == r);
  r = String8__cmp(s, s4);
  ASSERT(true == r);

  s2 = String8__cstr(arena, "bOb");
  r = String8__ncmp(s, s2);
  ASSERT(true == r);
  r = String8__cmp(s, s2);
  ASSERT(false == r);

  s = String8__cstr(arena, "hamster");
  s2 = String8__cstr(arena, "ste");
  s32 n;
  n = String8__indexOf(s, 1, s2);
  ASSERT(3 == n);
  n = String8__indexOf(s, 3, s2);
  ASSERT(3 == n);
  n = String8__indexOf(s, 4, s2);
  ASSERT(-1 == n);

  s3 = String8__cstr(arena, "ham");
  r = String8__startsWith(s, s3);
  ASSERT(true == r);
  s4 = String8__cstr(arena, "zzz");
  r = String8__startsWith(s, s4);
  ASSERT(false == r);

  String8__skip(s, 3);
  r = String8__startsWith(s, s2);
  ASSERT(true == r);
  r = String8__startsWith(s2, s);
  ASSERT(false == r);

  s = String8__cstr(arena, "Christmas");
  s2 = String8__substr(arena, s, 0, 5);
  ASSERT(5 == s2->sz);
  ASSERT('C' == s2->str[0]);

  s3 = String8__cstr(arena, "/var/log/test");
  List* l = String8__split(arena, s3, '/', 3);
  ASSERT(3 == l->len);
  s = List__get(l, 1);
  s2 = List__get(l, 2);
  ASSERT(3 == s->sz);
  ASSERT(true == String8__startsWith(s, String8__cstr(arena, "var")));
  ASSERT(3 == s2->sz);
  ASSERT(true == String8__startsWith(s2, String8__cstr(arena, "log")));

  ASSERT(String8__isSpaceC(' '));
  ASSERT(String8__isUpperC('Y'));
  ASSERT(String8__isLowerC('y'));
  ASSERT(String8__isAlphaC('x'));
  ASSERT(String8__isNumericC('3'));
  ASSERT(String8__isDigitC('.'));
  ASSERT(String8__isSlashC('\\'));
  ASSERT('v' == String8__toLowerC('V'));
  ASSERT('V' == String8__toUpperC('v'));
  ASSERT('/' == String8__toNixPathC('\\'));

  l = String8__split(arena, s3, '/', 0);
  s4 = String8__join(arena, l, '\\');
  ASSERT(true == String8__cmp(s4, String8__cstr(arena, "\\var\\log\\test")));

  return 0;
}