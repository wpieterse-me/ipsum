#include <cstdint>
#include <vector>
#include <fstream>

uint32_t pack_color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a = 255)
{
    return (a << 24) + (b << 16) + (g << 8) + r;
}

void unpack_color(const uint32_t &color, uint8_t &r, uint8_t &g, uint8_t &b, uint8_t &a)
{
    r = (color >> 0) & 255;
    g = (color >> 8) & 255;
    b = (color >> 16) & 255;
    a = (color >> 24) & 255;
}

void drop_ppm_image(const std::string filename, const std::vector<uint32_t> &image, const size_t w, const size_t h)
{
    std::ofstream ofs(filename, std::ios::binary);
    ofs << "P6" << std::endl
        << w << " " << h << std::endl
        << "255" << std::endl;

    for (size_t i = 0; i < (w * h); i++)
    {
        uint8_t r, g, b, a;

        unpack_color(image[i], r, g, b, a);

        ofs << static_cast<char>(r) << static_cast<char>(g) << static_cast<char>(b);
    }

    ofs.close();
}

void draw_rectangle(std::vector<uint32_t> &image, const size_t img_w, const size_t img_h, const size_t x, const size_t y, const size_t w, const size_t h, const uint32_t color)
{
    for (size_t i = 0; i < w; i++)
    {
        for (size_t j = 0; j < h; j++)
        {
            size_t cx = x + i;
            size_t cy = y + j;
            image[cx + cy * img_w] = color;
        }
    }
}

const size_t map_w = 16;
const size_t map_h = 16;

const char map[] = "0000222222220000"
                   "1              0"
                   "1      11111   0"
                   "1     0        0"
                   "0     0  1110000"
                   "0     3        0"
                   "0   10000      0"
                   "0   0   11100  0"
                   "0   0   0      0"
                   "0   0   1  00000"
                   "0       1      0"
                   "2       1      0"
                   "0       0      0"
                   "0 0000000      0"
                   "0              0"
                   "0002222222200000"; // our game map

float player_x = 3.456;
float player_y = 2.345;

int32_t main(int32_t argument_count, char **arguments)
{
    const size_t win_w = 512;
    const size_t win_h = 512;
    std::vector<uint32_t> framebuffer(win_w * win_h, 255);

    for (size_t j = 0; j < win_h; j++)
    {
        for (size_t i = 0; i < win_w; i++)
        {
            uint8_t r = 255 * j / float(win_h);
            uint8_t g = 255 * i / float(win_w);
            uint8_t b = 0;

            framebuffer[i + j * win_w] = pack_color(r, g, b);
        }
    }

    const size_t rect_w = win_w / map_w;
    const size_t rect_h = win_h / map_h;
    for (size_t j = 0; j < map_h; j++)
    {
        for (size_t i = 0; i < map_w; i++)
        {
            if (map[i + j * map_w] == ' ')
            {
                continue;
            }

            size_t rect_x = i * rect_w;
            size_t rect_y = j * rect_h;

            draw_rectangle(framebuffer, win_w, win_h, rect_x, rect_y, rect_w, rect_h, pack_color(0, 255, 255));
        }
    }

    draw_rectangle(framebuffer, win_w, win_h, player_x * rect_w, player_y * rect_h, 5, 5, pack_color(255, 255, 255));

    drop_ppm_image("out.ppm", framebuffer, win_w, win_h);
    return 0;
}
