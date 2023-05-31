#include "util.h"

uint32_t pack_color(const uint8_t red, const uint8_t green, const uint8_t blue, const uint8_t alpha)
{
    return (alpha << 24) + (blue << 16) + (green << 8) + red;
}
