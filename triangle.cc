#if defined(__SSE2__) || defined(__AVX2__)
#include <immintrin.h>
#endif

#include "triangle.h"

int32_t point_side(const point2d_t &v0, const point2d_t &v1, const point2d_t &v2)
{
    return (v1.x - v0.x) * (v2.y - v0.y) - (v1.y - v0.y) * (v2.x - v0.x);
}

void draw_triangle_general(uint32_t *image, int32_t image_width, int32_t image_height, const point2d_t &v0, const point2d_t &v1, const point2d_t &v2, uint32_t color)
{
    point2d_t current_point{0, 0};

    for (current_point.y = 0; current_point.y < image_height; current_point.y++)
    {
        for (current_point.x = 0; current_point.x < image_width; current_point.x++)
        {
            int32_t w0 = point_side(v1, v2, current_point);
            int32_t w1 = point_side(v2, v0, current_point);
            int32_t w2 = point_side(v0, v1, current_point);

            if (w0 >= 0 && w1 >= 0 && w2 >= 0)
            {
                image[current_point.x * image_width + current_point.y] = color;
            }
        }
    }
}

void draw_triangle_optimized_1(uint32_t *image, int32_t image_width, int32_t image_height, const point2d_t &v0, const point2d_t &v1, const point2d_t &v2, uint32_t color)
{
    int32_t a01 = v0.y - v1.y;
    int32_t b01 = v1.x - v0.x;
    int32_t a12 = v1.y - v2.y;
    int32_t b12 = v2.x - v1.x;
    int32_t a20 = v2.y - v0.y;
    int32_t b20 = v0.x - v2.x;

    point2d_t current_point{0, 0};
    int32_t w0_row = point_side(v1, v2, current_point);
    int32_t w1_row = point_side(v2, v0, current_point);
    int32_t w2_row = point_side(v0, v1, current_point);

    for (current_point.y = 0; current_point.y < image_height; current_point.y++)
    {
        int32_t w0 = w0_row;
        int32_t w1 = w1_row;
        int32_t w2 = w2_row;

        for (current_point.x = 0; current_point.x < image_width; current_point.x++)
        {
            if (w0 >= 0 && w1 >= 0 && w2 >= 0)
            {
                image[current_point.x * image_width + current_point.y] = color;
            }

            w0 += a12;
            w1 += a20;
            w2 += a01;
        }

        w0_row += b12;
        w1_row += b20;
        w2_row += b01;
    }
}

#if defined(__SSE2__)

void draw_triangle_sse2(uint32_t *image, int32_t image_width, int32_t image_height, const point2d_t &v0, const point2d_t &v1, const point2d_t &v2, uint32_t color)
{
}

#endif

#if defined(__AVX2__)
#endif
