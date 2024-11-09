#include "Sha1.h"

#include <string.h>

// const char *message = "Hello, SHA-1!";
// u8 hash[20];
// sha1(hash, (const u8 *)message, strlen(message));

// Helper function to print hash in hexadecimal format
// void print_hash(u8 *hash, size_t length) {
//     for (size_t i = 0; i < length; i++) {
//         printf("%02x", hash[i]);
//     }
//     printf("\n");
// }

// Define circular left rotate macro
#define LEFTROTATE(x, c) (((x) << (c)) | ((x) >> (32 - (c))))

// SHA-1 constants
#define SHA1_BLOCK_SIZE 64  // 512-bit blocks

// SHA-1 padding
static const u8 padding[SHA1_BLOCK_SIZE] = {
    0x80,
};

// SHA-1 initial hash values
static const u32 initial_hash[5] = {
    0x67452301,
    0xEFCDAB89,
    0x98BADCFE,
    0x10325476,
    0xC3D2E1F0,
};

// SHA-1 processing function
static void sha1_process_chunk(u32 state[5], const u8 chunk[SHA1_BLOCK_SIZE]) {
  u32 w[80];
  u32 a, b, c, d, e, f, k, temp;

  // Copy chunk into first 16 words w[0..15]
  for (int i = 0; i < 16; i++) {
    w[i] = (chunk[i * 4 + 0] << 24) | (chunk[i * 4 + 1] << 16) | (chunk[i * 4 + 2] << 8) |
           (chunk[i * 4 + 3]);
  }

  // Extend the first 16 words into the remaining 64 words
  for (int i = 16; i < 80; i++) {
    w[i] = LEFTROTATE((w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16]), 1);
  }

  // Initialize working variables with current hash value
  a = state[0];
  b = state[1];
  c = state[2];
  d = state[3];
  e = state[4];

  // Main loop
  for (int i = 0; i < 80; i++) {
    if (i < 20) {
      f = (b & c) | ((~b) & d);
      k = 0x5A827999;
    } else if (i < 40) {
      f = b ^ c ^ d;
      k = 0x6ED9EBA1;
    } else if (i < 60) {
      f = (b & c) | (b & d) | (c & d);
      k = 0x8F1BBCDC;
    } else {
      f = b ^ c ^ d;
      k = 0xCA62C1D6;
    }

    temp = LEFTROTATE(a, 5) + f + e + k + w[i];
    e = d;
    d = c;
    c = LEFTROTATE(b, 30);
    b = a;
    a = temp;
  }

  // Add the working variables back into the current hash value
  state[0] += a;
  state[1] += b;
  state[2] += c;
  state[3] += d;
  state[4] += e;
}

// SHA-1 finalization
void sha1(u8* hash, const u8* data, size_t len) {
  u32 state[5];
  memcpy(state, initial_hash, sizeof(initial_hash));

  u8 chunk[SHA1_BLOCK_SIZE];
  size_t i;

  // Process each 512-bit chunk
  for (i = 0; i + SHA1_BLOCK_SIZE <= len; i += SHA1_BLOCK_SIZE) {
    sha1_process_chunk(state, data + i);
  }

  // Remaining data less than a chunk, copy to the buffer
  size_t rem = len - i;
  memcpy(chunk, data + i, rem);

  // Append the bit '1' to the data (padding starts here)
  chunk[rem] = 0x80;
  rem++;

  // If not enough space for 64-bit length, process the current chunk and start a new one
  if (rem > 56) {
    memset(chunk + rem, 0, SHA1_BLOCK_SIZE - rem);
    sha1_process_chunk(state, chunk);
    rem = 0;
  }

  // Fill remaining space with zeros
  memset(chunk + rem, 0, 56 - rem);

  // Append the original message length in bits (big-endian)
  uint64_t bit_len = len * 8;
  chunk[56] = (bit_len >> 56) & 0xFF;
  chunk[57] = (bit_len >> 48) & 0xFF;
  chunk[58] = (bit_len >> 40) & 0xFF;
  chunk[59] = (bit_len >> 32) & 0xFF;
  chunk[60] = (bit_len >> 24) & 0xFF;
  chunk[61] = (bit_len >> 16) & 0xFF;
  chunk[62] = (bit_len >> 8) & 0xFF;
  chunk[63] = (bit_len) & 0xFF;

  sha1_process_chunk(state, chunk);

  // Produce the final hash value (big-endian)
  for (i = 0; i < 5; i++) {
    hash[i * 4 + 0] = (state[i] >> 24) & 0xFF;
    hash[i * 4 + 1] = (state[i] >> 16) & 0xFF;
    hash[i * 4 + 2] = (state[i] >> 8) & 0xFF;
    hash[i * 4 + 3] = (state[i]) & 0xFF;
  }
}