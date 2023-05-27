#pragma once

#include <cstdint>

struct point2d_t
{
    int32_t x;
    int32_t y;
};

int32_t point_side(const point2d_t &v0, const point2d_t &v1, const point2d_t &v2);
void draw_triangle(uint32_t *image, int32_t image_width, int32_t image_height, const point2d_t &v0, const point2d_t &v1, const point2d_t &v2, uint32_t color);
