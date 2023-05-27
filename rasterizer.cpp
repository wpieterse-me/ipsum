#include <cstdint>
#include <fstream>

#include "triangle.h"

uint32_t pack_color(const uint8_t red, const uint8_t green, const uint8_t blue, const uint8_t alpha = 255)
{
    return (alpha << 24) + (blue << 16) + (green << 8) + red;
}

void unpack_color(const uint32_t &color, uint8_t &red, uint8_t &green, uint8_t &blue, uint8_t &alpha)
{
    red = (color >> 0) & 255;
    green = (color >> 8) & 255;
    blue = (color >> 16) & 255;
    alpha = (color >> 24) & 255;
}

void write_framebuffer(const char *filename, const uint32_t *image, const int32_t image_width, const int32_t image_height)
{
    std::ofstream ofs(filename, std::ios::binary);
    ofs << "P6" << std::endl
        << image_width << " " << image_height << std::endl
        << "255" << std::endl;

    for (int32_t index = 0; index < (image_width * image_height); index++)
    {
        uint8_t red = 0;
        uint8_t green = 0;
        uint8_t blue = 0;
        uint8_t alpha = 0;

        unpack_color(image[index], red, green, blue, alpha);

        ofs << static_cast<char>(red) << static_cast<char>(green) << static_cast<char>(blue);
    }

    ofs.close();
}

void clear_image(uint32_t *image, int32_t image_width, int32_t image_height)
{
    for (int32_t index_y = 0; index_y < image_height; index_y++)
    {
        for (int32_t index_x = 0; index_x < image_width; index_x++)
        {
            image[index_x + index_y * image_width] = pack_color(0, 0, 0);
        }
    }
}

int32_t main(int32_t argument_count, char **arguments)
{
    static constexpr int32_t IMAGE_WIDTH = 2048;
    static constexpr int32_t IMAGE_HEIGHT = 2048;

    point2d_t v0{IMAGE_WIDTH, IMAGE_HEIGHT};
    point2d_t v1{0, IMAGE_HEIGHT};
    point2d_t v2{0, 0};
    uint32_t color = pack_color(255, 0, 0);

    uint32_t *image = new uint32_t[IMAGE_WIDTH * IMAGE_HEIGHT];

    clear_image(image, IMAGE_WIDTH, IMAGE_HEIGHT);
    draw_triangle_general(image, IMAGE_WIDTH, IMAGE_HEIGHT, v0, v1, v2, color);
    write_framebuffer("out_general.ppm", image, IMAGE_WIDTH, IMAGE_HEIGHT);

    clear_image(image, IMAGE_WIDTH, IMAGE_HEIGHT);
    draw_triangle_optimized_1(image, IMAGE_WIDTH, IMAGE_HEIGHT, v0, v1, v2, color);
    write_framebuffer("out_optimized_1.ppm", image, IMAGE_WIDTH, IMAGE_HEIGHT);

    clear_image(image, IMAGE_WIDTH, IMAGE_HEIGHT);
    draw_triangle_optimized_2(image, IMAGE_WIDTH, IMAGE_HEIGHT, v0, v1, v2, color);
    write_framebuffer("out_optimized_2.ppm", image, IMAGE_WIDTH, IMAGE_HEIGHT);

    return 0;
}
