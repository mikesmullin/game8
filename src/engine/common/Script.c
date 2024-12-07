#include "Script.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t Script__tokenize(const char* input, Script__Token* tokens, size_t max_tokens) {
  size_t token_count = 0;
  const char* current = input;

  tokens[token_count++] = (Script__Token){TOKEN_EOF, NULL};

  while (*current && token_count < max_tokens) {
    if (*current == '(') {
      tokens[token_count++] = (Script__Token){TOKEN_OPEN, NULL};
      current++;
    } else if (*current == ')') {
      tokens[token_count++] = (Script__Token){TOKEN_CLOSE, NULL};
      current++;
    } else if (isspace(*current)) {
      // Skip consecutive whitespace
      while (isspace(*current)) current++;
      // tokens[token_count++] = (Script__Token){TOKEN_WHITESPACE, NULL};
    } else if (isalpha(*current)) {
      // Parse WORD
      const char* start = current;
      size_t len = 0;

      while (isalnum(*current) || *current == '_') {
        current++;
        len++;
        if (len > 32) break;  // Max WORD length
      }

      char* word = (char*)malloc(len + 1);
      strncpy(word, start, len);
      word[len] = '\0';

      tokens[token_count++] = (Script__Token){TOKEN_WORD, word};
    } else if (isdigit(*current) || *current == '-' || *current == '+') {
      // Parse numeric literals (s32, u32, float, double)
      const char* start = current;
      size_t len = 0;

      while (isdigit(*current) || *current == '.' || *current == 'e' || *current == '-' ||
             *current == '+' || tolower(*current) == 'f' || tolower(*current) == 'u') {
        current++;
        len++;
      }

      char* literal = (char*)malloc(len + 1);
      strncpy(literal, start, len);
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
      while (*current && *current != '"' && *current != '\r' && *current != '\n') {
        current++;
      }

      if (*current == '"') {
        current++;  // Include the closing quote
      }

      size_t len = current - start;
      char* literal = (char*)malloc(len + 1);
      strncpy(literal, start, len);
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

void Script__lexer(
    Script__Token* tokens, size_t token_count, Script__Token* stack[], char* output) {
  size_t stack_top = 0;
  // TODO: stack via arena + list
  // TODO: accept Hash vtable sandbox scope
  // TODO: invoke vtable fn w/ reference to stack
  // TODO: vtable fns are wrappers that Script__pop() their args and cast them then forward to C ABI

  for (size_t i = token_count; i-- > 0;) {  // Process tokens right-to-left
    Script__Token* token = &tokens[i];

    switch (token->type) {
      case TOKEN_WORD:
      case TOKEN_S32:
      case TOKEN_U32:
      case TOKEN_FLOAT:
      case TOKEN_DOUBLE:
      case TOKEN_CHAR_PTR:
        stack[stack_top++] = token;
        break;

      case TOKEN_CLOSE:
      case TOKEN_OPEN:
      case TOKEN_EOF:
        for (size_t i = 0; i < stack_top; i++) {
          strcat(output, i == stack_top - 1 && token->type != TOKEN_CLOSE ? "call " : "push ");
          strcat(output, stack[i]->value);
          strcat(output, "\n");
        }
        stack_top = 0;
        break;

      default:
        break;
    }
  }
}

void Script__printTokens(Script__Token tokens[], u32 len) {
  printf("Tokens:\n");
  for (size_t i = 0; i < len; i++) {
    switch (tokens[i].type) {
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