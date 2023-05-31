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

/*
void draw_triangle_general(uint32_t *image, int32_t image_width, int32_t image_height, const point2d_t &v0, const point2d_t &v1, const point2d_t &v2, uint32_t color);
void draw_triangle_optimized_1(uint32_t *image, int32_t image_width, int32_t image_height, const point2d_t &v0, const point2d_t &v1, const point2d_t &v2, uint32_t color);
void draw_triangle_optimized_2(uint32_t *image, int32_t image_width, int32_t image_height, const point2d_t &v0, const point2d_t &v1, const point2d_t &v2, uint32_t color);
*/

/*
#if defined(__AVX2__)

void draw_triangle_avx2(uint32_t *image, int32_t image_width, int32_t image_height, const point2d_t &v0, const point2d_t &v1, const point2d_t &v2, uint32_t color);

#endif
*/
