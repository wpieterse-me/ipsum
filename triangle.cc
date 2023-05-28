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

void draw_triangle_optimized_2(uint32_t *image, int32_t image_width, int32_t image_height, const point2d_t &v0, const point2d_t &v1, const point2d_t &v2, uint32_t color)
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
            if ((w0 | w1 | w2) >= 0)
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

#if defined(__AVX2__)

struct edge_t
{
    static constexpr int32_t step_x_size = 8;
    static constexpr int32_t step_y_size = 1;

    __m256i one_step_x;
    __m256i one_step_y;

    __m256i initialize(const point2d_t &v0, const point2d_t &v1, const point2d_t &origin);
};

__m256i edge_t::initialize(const point2d_t &v0, const point2d_t &v1, const point2d_t &origin)
{
    const int32_t a_integral = v0.y - v1.y;
    const int32_t b_integral = v1.x - v0.x;
    const int32_t c_integral = v0.x * v1.y - v0.y * v1.x;

    one_step_x = _mm256_set1_epi32(a_integral * step_x_size);
    one_step_y = _mm256_set1_epi32(b_integral * step_y_size);

    const __m256i steps = _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7);
    const __m256i origin_x = _mm256_set1_epi32(origin.x);
    const __m256i x = _mm256_add_epi32(origin_x, steps);
    const __m256i y = _mm256_set1_epi32(origin.y);

    const __m256i a = _mm256_set1_epi32(a_integral);
    const __m256i b = _mm256_set1_epi32(b_integral);
    const __m256i c = _mm256_set1_epi32(c_integral);
    const __m256i a_mul_x = _mm256_mul_epi32(a, x);
    const __m256i b_mul_y = _mm256_mul_epi32(b, y);
    const __m256i ax_p_by = _mm256_add_epi32(a_mul_x, b_mul_y);
    const __m256i result = _mm256_add_epi32(ax_p_by, c);

    return result;
}

void draw_triangle_avx2(uint32_t *image, int32_t image_width, int32_t image_height, const point2d_t &v0, const point2d_t &v1, const point2d_t &v2, uint32_t color)
{
    point2d_t current_point{0, 0};

    edge_t e01{};
    edge_t e12{};
    edge_t e20{};

    __m256i zero = _mm256_set1_epi32(0);

    __m256i w0_row = e12.initialize(v1, v2, current_point);
    __m256i w1_row = e20.initialize(v2, v0, current_point);
    __m256i w2_row = e01.initialize(v0, v1, current_point);

    for (current_point.y = 0; current_point.y < image_height; current_point.y += edge_t::step_y_size)
    {
        __m256i w0 = w0_row;
        __m256i w1 = w1_row;
        __m256i w2 = w2_row;

        for (current_point.x = 0; current_point.x < image_width; current_point.x += edge_t::step_x_size)
        {
            const __m256i w0_or_w1 = _mm256_or_si256(w0, w1);
            const __m256i w0w1_or_w2 = _mm256_or_si256(w0_or_w1, w2);
            const __m256i cmp_gt = _mm256_cmpgt_epi32(w0w1_or_w2, zero);
            const __m256i cmp_eq = _mm256_cmpeq_epi32(w0w1_or_w2, zero);
            const __m256i cmp = _mm256_or_si256(cmp_gt, cmp_eq);
            const int32_t mask = _mm256_movemask_ps((__m256)cmp);

            if (mask & 1)
            {
                image[(current_point.x + 0) * image_width + current_point.y] = color;
            }

            if (mask & 2)
            {
                image[(current_point.x + 1) * image_width + current_point.y] = color;
            }

            if (mask & 4)
            {
                image[(current_point.x + 2) * image_width + current_point.y] = color;
            }

            if (mask & 8)
            {
                image[(current_point.x + 3) * image_width + current_point.y] = color;
            }

            if (mask & 16)
            {
                image[(current_point.x + 4) * image_width + current_point.y] = color;
            }

            if (mask & 32)
            {
                image[(current_point.x + 5) * image_width + current_point.y] = color;
            }

            if (mask & 64)
            {
                image[(current_point.x + 6) * image_width + current_point.y] = color;
            }

            if (mask & 128)
            {
                image[(current_point.x + 7) * image_width + current_point.y] = color;
            }

            w0 = _mm256_add_epi32(w0, e12.one_step_x);
            w1 = _mm256_add_epi32(w1, e20.one_step_x);
            w2 = _mm256_add_epi32(w2, e01.one_step_x);
        }

        w0_row = _mm256_add_epi32(w0_row, e12.one_step_y);
        w1_row = _mm256_add_epi32(w1_row, e20.one_step_y);
        w2_row = _mm256_add_epi32(w2_row, e01.one_step_y);
    }
}

#endif
