#include "util.h"
#include "triangle.h"

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

void draw_triangle_trenki2_p1(uint32_t *image, int32_t image_width, int32_t image_height, const point2d_t &v0, const point2d_t &v1, const point2d_t &v2)
{
    edge_equation_s e0(v1, v2);
    edge_equation_s e1(v2, v0);
    edge_equation_s e2(v0, v1);

    float area = 0.5f * (e0.c + e1.c + e2.c);

    if (area < 0.0f)
    {
        return;
    }

    parameter_equation_s r(v0.r, v1.r, v2.r, e0, e1, e2, area);
    parameter_equation_s g(v0.g, v1.g, v2.g, e0, e1, e2, area);
    parameter_equation_s b(v0.b, v1.b, v2.b, e0, e1, e2, area);

    for (float x = 0.5f; x < ((float)image_width) + 0.5f; x += 1.0f)
    {
        for (float y = 0.5f; y < ((float)image_height) + 0.5f; y += 1.0f)
        {
            if (e0.test(x, y) && e1.test(x, y) && e2.test(x, y))
            {
                int32_t x_coord = (int32_t)x;
                int32_t y_coord = (int32_t)y;

                int32_t r_color = (int32_t)(r.evaluate(x, y) * 255);
                int32_t g_color = (int32_t)(g.evaluate(x, y) * 255);
                int32_t b_color = (int32_t)(b.evaluate(x, y) * 255);

                image[x_coord * image_width + y_coord] = pack_color(r_color, g_color, b_color);
            }
        }
    }
}
