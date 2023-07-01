#include "util.h"

uint32_t pack_color(const uint8_t red,
                    const uint8_t green,
                    const uint8_t blue,
                    const uint8_t alpha) {
  return (alpha << 24) + (blue << 16) + (green << 8) + red;
}

void unpack_color(const uint32_t color,
                  uint8_t*       r,
                  uint8_t*       g,
                  uint8_t*       b,
                  uint8_t*       a) {
  *r = (color >> 0) * 255;
  *g = (color >> 8) * 255;
  *b = (color >> 16) * 255;
  *a = (color >> 24) * 255;
}
