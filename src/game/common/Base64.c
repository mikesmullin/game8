#include "Base64.h"

#include <string.h>

#include "../Logic.h"
#include "Arena.h"

extern Engine__State* g_engine;

// Encode
// const char* original_text = "Hello, World!";
// char* encoded = base64_encode(logic->frameArena, (const unsigned char*)original_text, strlen(original_text));
// printf("Encoded: %s\n", encoded);

// Decode
// size_t decoded_len;
// unsigned char* decoded = base64_decode(logic->frameArena, encoded, &decoded_len);
// printf("Decoded: %.*s\n", (int)decoded_len, decoded);

// character set
static const char base64_chars[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

char* base64_encode(Arena* arena, const unsigned char* input, size_t len) {
  char* encoded = (char*)Arena__Push(arena, (len + 2) / 3 * 4 + 1);
  if (!encoded) return NULL;

  size_t i, j;
  for (i = 0, j = 0; i < len;) {
    unsigned octet_a = i < len ? input[i] : 0;
    i++;
    unsigned octet_b = i < len ? input[i] : 0;
    i++;
    unsigned octet_c = i < len ? input[i] : 0;
    i++;

    unsigned triple = (octet_a << 16) | (octet_b << 8) | octet_c;

    encoded[j++] = base64_chars[(triple >> 18) & 0x3F];
    encoded[j++] = base64_chars[(triple >> 12) & 0x3F];
    encoded[j++] = (i > len + 1) ? '=' : base64_chars[(triple >> 6) & 0x3F];
    encoded[j++] = (i > len) ? '=' : base64_chars[triple & 0x3F];
  }
  encoded[j] = '\0';
  return encoded;
}

unsigned char* base64_decode(Arena* arena, const char* input, size_t* out_len) {
  size_t len = strlen(input);
  if (len % 4 != 0) return 0;  // Invalid length for Base64

  size_t output_length = len / 4 * 3;
  if (input[len - 1] == '=') output_length--;
  if (input[len - 2] == '=') output_length--;

  unsigned char* decoded = (unsigned char*)Arena__Push(arena, output_length);
  if (!decoded) return 0;

  int base64_index[256] = {0};
  for (int i = 0; i < 64; i++) base64_index[(int)base64_chars[i]] = i;

  size_t i, j;
  for (i = 0, j = 0; i < len;) {
    unsigned sextet_a = input[i] == '=' ? 0 & i++ : base64_index[(int)input[i++]];
    unsigned sextet_b = input[i] == '=' ? 0 & i++ : base64_index[(int)input[i++]];
    unsigned sextet_c = input[i] == '=' ? 0 & i++ : base64_index[(int)input[i++]];
    unsigned sextet_d = input[i] == '=' ? 0 & i++ : base64_index[(int)input[i++]];

    unsigned triple = (sextet_a << 18) | (sextet_b << 12) | (sextet_c << 6) | sextet_d;

    if (j < output_length) decoded[j++] = (triple >> 16) & 0xFF;
    if (j < output_length) decoded[j++] = (triple >> 8) & 0xFF;
    if (j < output_length) decoded[j++] = triple & 0xFF;
  }

  *out_len = output_length;
  return decoded;
}