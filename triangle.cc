#include "triangle.h"

int32_t point_side(const point2d_t &v0, const point2d_t &v1, const point2d_t &v2)
{
    return (v1.x - v0.x) * (v2.y - v0.y) - (v1.y - v0.y) * (v2.x - v0.x);
}

void draw_triangle(uint32_t *image, int32_t image_width, int32_t image_height, const point2d_t &v0, const point2d_t &v1, const point2d_t &v2, uint32_t color)
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
