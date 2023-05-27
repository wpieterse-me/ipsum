#include <cstdint>
#include <fstream>

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

int32_t main(int32_t argument_count, char **arguments)
{
    static constexpr int32_t FRAMEBUFFER_WIDTH = 512;
    static constexpr int32_t FRAMEBUFFER_HEIGHT = 512;

    uint32_t *framebuffer = new uint32_t[FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT];

    for (int32_t index_y = 0; index_y < FRAMEBUFFER_HEIGHT; index_y++)
    {
        for (int32_t index_x = 0; index_x < FRAMEBUFFER_WIDTH; index_x++)
        {
            uint8_t red = 255 * index_y / FRAMEBUFFER_HEIGHT;
            uint8_t green = 255 * index_x / FRAMEBUFFER_WIDTH;

            framebuffer[index_x + index_y * FRAMEBUFFER_WIDTH] = pack_color(red, green, 0);
        }
    }

    write_framebuffer("out.ppm", framebuffer, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);

    return 0;
}
