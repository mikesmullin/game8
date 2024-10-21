#include "Color.h"

u32 alpha_blend(u32 bottom, u32 top) {
  // Extract RGBA components
  u32 src_a = (top >> 24) & 0xff;
  u32 src_r = (top >> 16) & 0xff;
  u32 src_g = (top >> 8) & 0xff;
  u32 src_b = top & 0xff;

  u32 dst_a = (bottom >> 24) & 0xff;
  u32 dst_r = (bottom >> 16) & 0xff;
  u32 dst_g = (bottom >> 8) & 0xff;
  u32 dst_b = bottom & 0xff;

  // Normalize alpha values to [0, 1]
  f32 alpha_src = src_a / 255.0f;
  f32 alpha_dst = dst_a / 255.0f;

  // Apply alpha blending formula
  u32 out_r = (u32)((alpha_src * src_r) + ((1 - alpha_src) * dst_r));
  u32 out_g = (u32)((alpha_src * src_g) + ((1 - alpha_src) * dst_g));
  u32 out_b = (u32)((alpha_src * src_b) + ((1 - alpha_src) * dst_b));
  u32 out_a = (u32)((alpha_src * 255.0f) + ((1 - alpha_src) * alpha_dst * 255.0f));

  // Combine components back into a single u32
  u32 result = (out_a << 24) | (out_r << 16) | (out_g << 8) | out_b;
  return result;
}