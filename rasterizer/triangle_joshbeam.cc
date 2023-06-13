#include "util.h"
#include "rasterizer/triangle.h"

struct span_t
{
    point2d_t v0;
    point2d_t v1;

    span_t(point2d_t v0_in, point2d_t v1_in)
    {
        if (v0_in.x < v1_in.x)
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

static void draw_triangle_span(uint32_t *image, int32_t image_width, int32_t image_height, span_t span)
{
    float y = span.v0.y;

    float xdiff = span.v1.x - span.v0.x;
    if (xdiff == 0)
    {
        return;
    }

    float colordiff_r = span.v1.r - span.v0.r;
    float colordiff_g = span.v1.g - span.v0.g;
    float colordiff_b = span.v1.b - span.v0.b;
    float factor = 0.0f;
    float factor_step = 1.0f / xdiff;

    for (float x = span.v0.x; x < span.v1.x; x += 1.0f)
    {
        float color_r = span.v0.r + (colordiff_r * factor);
        float color_g = span.v0.g + (colordiff_g * factor);
        float color_b = span.v0.b + (colordiff_b * factor);

        int32_t x_coord = static_cast<int32_t>(x);
        int32_t y_coord = static_cast<int32_t>(y);
        uint32_t r = static_cast<uint32_t>(color_r * 255);
        uint32_t g = static_cast<uint32_t>(color_g * 255);
        uint32_t b = static_cast<uint32_t>(color_b * 255);

        image[x_coord * image_width + y_coord] = pack_color(r, g, b);

        factor += factor_step;
    }
}

static void draw_triangle_span_edge(uint32_t *image, int32_t image_width, int32_t image_height, span_edge_t &e0, span_edge_t &e1)
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
    float e0colordiff_r = e0.v1.r - e0.v0.r;
    float e0colordiff_g = e0.v1.g - e0.v0.g;
    float e0colordiff_b = e0.v1.b - e0.v0.b;
    float e0factor = static_cast<float>(e1.v0.y - e0.v0.y) / e0ydiff;
    float e0factor_step = 1.0f / e0ydiff;

    float e1xdiff = e1.v1.x - e1.v0.x;
    float e1colordiff_r = e1.v1.r - e1.v0.r;
    float e1colordiff_g = e1.v1.g - e1.v0.g;
    float e1colordiff_b = e1.v1.b - e1.v0.b;
    float e1factor = 0.0f;
    float e1factor_step = 1.0f / e1ydiff;

    for (float y = e1.v0.y; y < e1.v1.y; y += 1.0f)
    {
        point2d_t start{
            e0.v0.x + static_cast<int32_t>(e0xdiff * e0factor),
            y,
            e0.v0.r + (e0colordiff_r * e0factor),
            e0.v0.g + (e0colordiff_g * e0factor),
            e0.v0.b + (e0colordiff_b * e0factor)};

        point2d_t end{
            e1.v0.x + static_cast<int32_t>(e1xdiff * e1factor),
            y,
            e1.v0.r + (e1colordiff_r * e1factor),
            e1.v0.g + (e1colordiff_g * e1factor),
            e1.v0.b + (e1colordiff_b * e1factor)};

        span_t span{start, end};

        draw_triangle_span(image, image_width, image_height, span);

        e0factor += e0factor_step;
        e1factor += e1factor_step;
    }
}

void draw_triangle_joshbeam(uint32_t *image, int32_t image_width, int32_t image_height, const point2d_t &v0, const point2d_t &v1, const point2d_t &v2)
{
    span_edge_t edges[] = {
        span_edge_t{v0, v1},
        span_edge_t{v1, v2},
        span_edge_t{v2, v0},
    };

    float max_length = 0;
    int32_t long_edge = 0;

    for (int32_t index = 0; index < 3; index++)
    {
        float length = edges[index].v1.y - edges[index].v0.y;
        if (length > max_length)
        {
            max_length = length;
            long_edge = index;
        }
    }

    int32_t short_edge_1 = (long_edge + 1) % 3;
    int32_t short_edge_2 = (long_edge + 2) % 3;

    draw_triangle_span_edge(image, image_width, image_height, edges[long_edge], edges[short_edge_1]);
    draw_triangle_span_edge(image, image_width, image_height, edges[long_edge], edges[short_edge_2]);
}
