#pragma once

#include <cstdint>

struct point2d_t
{
    float x;
    float y;

    float r;
    float g;
    float b;
};

void draw_triangle_joshbeam(uint32_t *image, int32_t image_width, int32_t image_height, const point2d_t &v0, const point2d_t &v1, const point2d_t &v2);
void draw_triangle_trenki2_p1(uint32_t *image, int32_t image_width, int32_t image_height, const point2d_t &v0, const point2d_t &v1, const point2d_t &v2);
