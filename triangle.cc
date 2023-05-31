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

struct span_t
{
    int32_t v0_x;
    int32_t v1_x;

    span_t(int32_t v0_x_in, int32_t v1_x_in)
    {
        if (v0_x_in < v1_x_in)
        {
            v0_x = v0_x_in;
            v1_x = v1_x_in;
        }
        else
        {
            v0_x = v1_x_in;
            v1_x = v0_x_in;
        }
    }
};

struct span_edge_t
{
    point2d_t v0;
    point2d_t v1;

    span_edge_t(point2d_t v0_in, point2d_t v1_in)
    {
        if (v0_in.y < v1_in.y)
        {
            v0 = v0_in;
            v1 = v1_in;
        }
        else
        {
            v0 = v1_in;
            v1 = v0_in;
        }
    }
};

void draw_triangle_span(uint32_t *image, int32_t image_width, int32_t image_height, span_t span, int32_t y, uint32_t color)
{
    int32_t xdiff = span.v1_x - span.v0_x;
    if (xdiff == 0)
    {
        return;
    }

    float factor = 0.0f;
    float factor_step = 1.0f / xdiff;

    for (int32_t x = span.v0_x; x < span.v1_x; x++)
    {
        image[x * image_width + y] = color;

        factor += factor_step;
    }
}

void draw_triangle_span_edge(uint32_t *image, int32_t image_width, int32_t image_height, span_edge_t &e0, span_edge_t &e1, uint32_t color)
{
    float e0ydiff = e0.v1.y - e0.v0.y;
    if (e0ydiff == 0.0f)
    {
        return;
    }

    float e1ydiff = e1.v1.y - e1.v0.y;
    if (e1ydiff == 0.0f)
    {
        return;
    }

    float e0xdiff = e0.v1.x - e0.v0.x;
    float e1xdiff = e1.v1.x - e1.v0.x;

    float factor_1 = static_cast<float>(e1.v0.y - e0.v0.y) / e0ydiff;
    float factor_1_step = 1.0f / e0ydiff;
    float factor_2 = 0.0f;
    float factor_2_step = 1.0f / e1ydiff;

    for (int32_t y = e1.v0.y; y < e1.v1.y; y++)
    {
        int32_t x_start = e0.v0.x + static_cast<int32_t>(e0xdiff * factor_1);
        int32_t x_end = e1.v0.x + static_cast<int32_t>(e1xdiff * factor_2);

        span_t span{x_start, x_end};

        draw_triangle_span(image, image_width, image_height, span, y, color);

        factor_1 += factor_1_step;
        factor_2 += factor_2_step;
    }
}

void draw_triangle_span(uint32_t *image, int32_t image_width, int32_t image_height, const point2d_t &v0, const point2d_t &v1, const point2d_t &v2, uint32_t color)
{
    span_edge_t edges[] = {
        span_edge_t{v0, v1},
        span_edge_t{v1, v2},
        span_edge_t{v2, v0},
    };

    int32_t max_length = 0;
    int32_t long_edge = 0;

    for (int32_t index = 0; index < 3; index++)
    {
        int32_t length = edges[index].v1.y - edges[index].v0.y;
        if (length > max_length)
        {
            max_length = length;
            long_edge = index;
        }
    }

    int32_t short_edge_1 = (long_edge + 1) % 3;
    int32_t short_edge_2 = (long_edge + 2) % 3;

    draw_triangle_span_edge(image, image_width, image_height, edges[long_edge], edges[short_edge_1], color);
    draw_triangle_span_edge(image, image_width, image_height, edges[long_edge], edges[short_edge_2], color);
}

struct edge_equation_s
{
    float a;
    float b;
    float c;
    bool tie;

    edge_equation_s(const point2d_t &v0, const point2d_t &v1)
    {
        a = v0.y - v1.y;
        b = v1.x - v0.x;
        c = -(a * (v0.x + v1.x) + b * (v0.y + v1.y)) / 2;
        tie = a != 0 ? a > 0 : b > 0;
    }

    float evaluate(float x, float y)
    {
        return a * x + b * y + c;
    }

    bool test(float x, float y)
    {
        return test(evaluate(x, y));
    }

    bool test(float v)
    {
        return (v > 0 || v == 0 && tie);
    }
};

struct parameter_equation_s
{
    float a;
    float b;
    float c;

    parameter_equation_s(float p0, float p1, float p2, const edge_equation_s &e0, const edge_equation_s &e1, const edge_equation_s &e2, float area)
    {
        float factor = 1.0f / (2.0f * area);

        a = factor * (p0 * e0.a + p1 * e1.a + p2 * e2.a);
        b = factor * (p0 * e0.b + p1 * e1.b + p2 * e2.b);
        c = factor * (p0 * e0.c + p1 * e1.c + p2 * e2.c);
    }

    float evaluate(float x, float y)
    {
        return a * x + b * y + c;
    }
};

void draw_triangle_trenki2(uint32_t *image, int32_t image_width, int32_t image_height, const point2d_t &v0, const point2d_t &v1, const point2d_t &v2, uint32_t color)
{
    edge_equation_s e0(v1, v2);
    edge_equation_s e1(v2, v0);
    edge_equation_s e2(v0, v1);

    float area = 0.5f * (e0.c + e1.c + e2.c);

    if (area < 0.0f)
    {
        return;
    }

    for (int32_t x = 0; x < image_width; x++)
    {
        for (int32_t y = 0; y < image_height; y++)
        {
            if (e0.test(x, y) && e1.test(x, y) && e2.test(x, y))
            {
                image[x * image_width + y] = color;
            }
        }
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
