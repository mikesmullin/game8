#include "Script.h"

#include "Arena.h"
#include "Hash.h"
#include "List.h"
#include "Log.h"

#define SCRIPT_ASSERT(cond, ctx, ...)                                      \
  if (!(cond)) {                                                           \
    g_engine->log(                                                         \
        ("Assertion failed: " #cond "\n  at %s:%u\n  Context: " ctx "\n"), \
        __FILE__,                                                          \
        __LINE__,                                                          \
        ##__VA_ARGS__);                                                    \
    return;                                                                \
  }

static inline bool isSpace(char c) {
  return ' ' == c || '\t' == c || '\r' == c || '\n' == c;
}

static inline bool isAlpha(char c) {
  return (c > 64 && c < 91) || (c > 96 && c < 123);
}

static inline bool isNumeric(char c) {
  return c > 47 && c < 58;
}

static inline bool isAlphaNumeric(char c) {
  return isAlpha(c) || isNumeric(c);
}

static inline bool isDigit(char c) {
  return isNumeric(c) || '.' == c;
}

u32 Script__tokenize(const char* input, Script__Token* tokens, u32 max_tokens) {
  u32 token_count = 0;
  const char* current = input;

  tokens[token_count++] = (Script__Token){TOKEN_EOF, NULL};

  while (*current && token_count < max_tokens) {
    if (*current == '(') {
      tokens[token_count++] = (Script__Token){TOKEN_OPEN, NULL};
      current++;
    } else if (*current == ')') {
      tokens[token_count++] = (Script__Token){TOKEN_CLOSE, NULL};
      current++;
    } else if (*current == ',') {
      // ignore comma separators
      current++;
    } else if (isSpace(*current)) {
      // Skip consecutive whitespace
      while (isSpace(*current)) current++;
      // tokens[token_count++] = (Script__Token){TOKEN_WHITESPACE, NULL};
    } else if (isAlpha(*current)) {
      // Parse WORD
      const char* start = current;
      u32 len = 0;

      while (isAlphaNumeric(*current) || *current == '_') {
        current++;
        len++;
        if (len > 32) break;  // Max WORD length
      }

      char* word = (char*)malloc(len + 1);
      mmemcp(word, start, len);
      word[len] = '\0';

      tokens[token_count++] = (Script__Token){TOKEN_WORD, word};
    } else if (isDigit(*current) || *current == '-' || *current == '+') {
      // Parse numeric literals (s32, u32, float, double)
      const char* start = current;
      u32 len = 0;

      while (isDigit(*current) || *current == '.' || *current == 'e' || *current == '-' ||
             *current == '+' || tolower(*current) == 'f' || tolower(*current) == 'u') {
        current++;
        len++;
      }

      char* literal = (char*)malloc(len + 1);
      mmemcp(literal, start, len);
      literal[len] = '\0';

      // Determine token type
      if (strchr(literal, '.')) {
        if (tolower(literal[len - 1]) == 'f') {
          tokens[token_count++] = (Script__Token){TOKEN_FLOAT, literal};
        } else {
          tokens[token_count++] = (Script__Token){TOKEN_DOUBLE, literal};
        }
      } else if (tolower(literal[len - 1]) == 'u') {
        tokens[token_count++] = (Script__Token){TOKEN_U32, literal};
      } else {
        tokens[token_count++] = (Script__Token){TOKEN_S32, literal};
      }
    } else if (*current == '"') {
      // Parse string literals
      const char* start = current++;
      while (*current && !(*current == '"' && *(current - 1) != '\\') && *current != '\r' &&
             *current != '\n') {
        current++;
      }

      if (*current == '"') {
        current++;  // Include the closing quote
      }

      u32 len = current - start;
      char* literal = (char*)malloc(len + 1);
      mmemcp(literal, start, len);
      literal[len] = '\0';

      tokens[token_count++] = (Script__Token){TOKEN_CHAR_PTR, literal};
    } else {
      // Unknown token
      tokens[token_count++] = (Script__Token){TOKEN_UNKNOWN, NULL};
      current++;
    }
  }

  return token_count;
}

void Script__exec(
    Arena* arena, Script__Token* tokens, u32 token_count, HashTable* vtable, List* stack) {
  for (u32 i = token_count; i-- > 0;) {  // Process tokens right-to-left
    Script__Token* token = &tokens[i];

    switch (token->type) {
      case TOKEN_WORD:
      case TOKEN_S32:
      case TOKEN_U32:
      case TOKEN_FLOAT:
      case TOKEN_DOUBLE:
      case TOKEN_CHAR_PTR:
        List__append(arena, stack, token);  // push
        break;

      case TOKEN_OPEN:
      case TOKEN_EOF:
        if (0 == stack->len) break;
        token = List__pop(stack);  // fn
        SCRIPT_ASSERT(TOKEN_WORD == token->type, "Invalid fn call.");
        HashTable_Node* node = HashTable__get(vtable, token->value);
        SCRIPT_ASSERT(NULL != node, "Undefined fn: %s", token->value);
        ((Script__fn_t)node->value)(arena, vtable, stack);
        break;

      default:
        break;
    }
  }
}

void Script__printTokens(Script__Token tokens[], u32 len) {
  printf("Tokens:\n");
  for (u32 i = 0; i < len; i++) {
    switch (tokens[i].type) {
      case TOKEN_EOF:
        printf("TOKEN_EOF\n");
        break;
      case TOKEN_OPEN:
        printf("TOKEN_OPEN\n");
        break;
      case TOKEN_CLOSE:
        printf("TOKEN_CLOSE\n");
        break;
      case TOKEN_WORD:
        printf("TOKEN_WORD: %s\n", tokens[i].value);
        break;
      case TOKEN_S32:
        printf("TOKEN_S32: %s\n", tokens[i].value);
        break;
      case TOKEN_U32:
        printf("TOKEN_U32: %s\n", tokens[i].value);
        break;
      case TOKEN_FLOAT:
        printf("TOKEN_FLOAT: %s\n", tokens[i].value);
        break;
      case TOKEN_DOUBLE:
        printf("TOKEN_DOUBLE: %s\n", tokens[i].value);
        break;
      case TOKEN_CHAR_PTR:
        printf("TOKEN_CHAR_PTR: %s\n", tokens[i].value);
        break;
      default:
        printf("TOKEN_UNKNOWN\n");
        break;
    }
  }
}