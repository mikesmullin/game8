#pragma once

#include "Types.h"

typedef enum Script__TokenType {
  TOKEN_UNKNOWN,
  TOKEN_OPEN,  // '('
  TOKEN_CLOSE,  // ')'
  TOKEN_WORD,  // WORD
  TOKEN_S32,  // Signed 32-bit integer
  TOKEN_U32,  // Unsigned 32-bit integer
  TOKEN_FLOAT,  // Floating-point number with 'f'
  TOKEN_DOUBLE,  // Double-precision floating-point
  TOKEN_CHAR_PTR,  // String literal
  TOKEN_EOF,
} Script__TokenType;

typedef struct Script__Token {
  Script__TokenType type;
  char* value;  // dynamically allocated for WORD and literals
} Script__Token;

typedef enum Script__TokenType2 {
  TOKEN2_PUSH,
  TOKEN2_CALL,
} Script__TokenType2;

typedef enum Script__TokenType3 {
  TOKEN3_POINTER,
  TOKEN3_FLOAT,
  TOKEN3_U32,
  TOKEN3_S32,
} Script__TokenType3;

typedef union Script__Token4 {
  void* p;
  f32 f;
  u32 u;
  s32 s;
} Script__Token4;

typedef struct Script__Token3 {
  Script__TokenType3 type;
  Script__Token4 value;
} Script__Token3;

typedef struct Script__Token2 {
  Script__TokenType2 opcode;
  Script__Token3 operand1;
} Script__Token2;

typedef struct Arena Arena;
typedef struct List List;
typedef struct HashTable HashTable;

typedef void (*Script__fn_t)(Arena* arena, const HashTable* vtable, List* stack);

void Script__printTokens(Script__Token tokens[], u32 len);
size_t Script__tokenize(const char* input, Script__Token* tokens, size_t max_tokens);
void Script__exec(
    Arena* arena, Script__Token* tokens, u32 token_count, HashTable* vtable, List* stack);