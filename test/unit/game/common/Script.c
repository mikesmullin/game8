#define ENGINE__NO_MAIN
#include "../../../../src/engine/Engine.h"  // IWYU pragma: keep

//

#include "../../../../src/engine/common/Hash.h"
#include "../../../../src/engine/common/Math.h"
#include "../../../../src/engine/common/Script.h"

#define MAX_TOKENS 1024
#define MAX_OUTPUT 2048

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

static u32 abcd(s32 a, u32 b, f32 c, void* d) {
  return (u32)a + (u32)b + (u32)c + (u32)d;
}

// void call_function_with_asm(void* func, s32 a, u32 b, f32 c, void* d) {
//   __asm__ volatile(
//       "mov %[a], %%edi\n"  // Move first argument (a) into RDI
//       "mov %[b], %%esi\n"  // Move second argument (b) into RSI
//       "mov %[c], %%edx\n"  // Move third argument (c) into RDX
//       "mov %[d], %%rcx\n"  // Move fourth argument (d) into RCX
//       "call *%[func]\n"  // Call the function pointer
//       :  // No output operands
//       : [func] "r"(func),  // Input operands
//         [a] "r"(a),
//         [b] "r"(b),
//         [c] "r"(c),
//         [d] "r"(d)
//       : "edi", "esi", "edx", "rcx", "memory");  // Clobbered registers
// }

void callsufp(void* fn, s32 a, u32 b, f32 c, void* d) {
  (void)((void (*)(s32, u32, f32, void*))fn)(a, b, c, d);
}

static void abcdE(void* a, void* b, void* c, void* d) {
  (void)abcd(*(s32*)a, *(u32*)b, *(f32*)c, d);
}

void call4(void* fn, void* a, void* b, void* c, void* d) {
  (void)((void (*)(void*, void*, void*, void*))fn)(a, b, c, d);
}

// @describe Script
// @tag common
int main() {
  const char* source_code;
  // source_code = "(define x -1234 1.0 3.14e10f \"Hello, World!\" 42u)";
  source_code = "set (Level__findEntity (getTag TAG_CATSPAWN)) 1u 1.0f \"ab\\n\"";

  Script__Token tokens[MAX_TOKENS];
  size_t token_count = Script__tokenize(source_code, tokens, MAX_TOKENS);
  Script__printTokens(tokens, token_count);

  char output[MAX_OUTPUT] = {0};
  Script__Token* stack[MAX_TOKENS];
  Script__lexer(tokens, token_count, stack, output);
  printf("Assembly:\n");
  printf("%s", output);

  Script__Token2 tokens2[] = {
      (Script__Token2){TOKEN2_PUSH, (Script__Token3){.type = TOKEN3_POINTER, .value.p = 0}},
      (Script__Token2){TOKEN2_PUSH, (Script__Token3){.type = TOKEN3_FLOAT, .value.f = 1.0f}},
      (Script__Token2){TOKEN2_PUSH, (Script__Token3){.type = TOKEN3_U32, .value.u = 2}},
      (Script__Token2){TOKEN2_PUSH, (Script__Token3){.type = TOKEN3_S32, .value.s = -3}},
      (Script__Token2){TOKEN2_CALL, (Script__Token3){.type = TOKEN3_POINTER, .value.p = &abcdE}},
  };

  // call_function_with_asm((void*)&abcd, -3, 2, 1.0f, &value);
  // callsufp(
  //     tokens2[4].operand1.value.p,
  //     tokens2[3].operand1.value.s,
  //     tokens2[2].operand1.value.u,
  //     tokens2[1].operand1.value.f,
  //     tokens2[0].operand1.value.p);

  call4(
      tokens2[4].operand1.value.p,
      &tokens2[3].operand1.value.s,
      &tokens2[2].operand1.value.u,
      &tokens2[1].operand1.value.f,
      tokens2[0].operand1.value.p);

  return 0;
}