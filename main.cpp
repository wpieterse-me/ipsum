#include <cstdint>

#include <SDL2/SDL.h>

static constexpr int32_t WINDOW_WIDTH = 640;
static constexpr int32_t WINDOW_HEIGHT = 480;

void DrawPixel(uint8_t *framebuffer, int32_t x, int32_t y, uint32_t color)
{
    uint32_t *extended_framebuffer = (uint32_t *)framebuffer;

    extended_framebuffer[y * WINDOW_WIDTH + x] = color;
}

void DrawStraightLine(uint8_t *framebuffer, int32_t start_x, int32_t end_x, int32_t y, uint32_t color)
{
    for (int32_t counter = start_x; counter < end_x; counter++)
    {
        DrawPixel(framebuffer, counter, y, color);
    }
}

struct Point2D
{
    int32_t x;
    int32_t y;
};

int32_t PointSide(const Point2D &a, const Point2D &b, const Point2D &c)
{
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

void DrawTriangle(size_t window_width, size_t window_height, uint8_t *framebuffer, const Point2D &v0, const Point2D &v1, const Point2D &v2)
{
    Point2D p;
    for (p.y = 0; p.y < window_height; p.y++)
    {
        for (p.x = 0; p.x < window_width; p.x++)
        {
            int32_t w0 = PointSide(v1, v2, p);
            int32_t w1 = PointSide(v2, v0, p);
            int32_t w2 = PointSide(v0, v1, p);

            if (w0 >= 0 && w1 >= 0 && w2 >= 0)
            {
                DrawPixel(framebuffer, p.x, p.y, 0xFFFFFFFF);
            }
        }
    }
}

int32_t main(int32_t argument_count, char **arguments)
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("SDL2Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH, WINDOW_HEIGHT);
    uint8_t *pixels = new uint8_t[WINDOW_WIDTH * WINDOW_HEIGHT * 4];

    Point2D a{WINDOW_WIDTH - 10, WINDOW_HEIGHT - 10};
    Point2D b{10, WINDOW_HEIGHT - 10};
    Point2D c{10, 10};

    bool running = true;
    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
        }

        DrawTriangle(WINDOW_WIDTH, WINDOW_HEIGHT, pixels, a, b, c);

        SDL_UpdateTexture(texture, nullptr, pixels, WINDOW_WIDTH * 4);

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }

    delete[] pixels;
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    return 0;
}
