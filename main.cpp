#include <cstdint>

#include <SDL2/SDL.h>

static constexpr int32_t WINDOW_WIDTH = 640;
static constexpr int32_t WINDOW_HEIGHT = 480;

void DrawPixel(uint8_t *framebuffer, int32_t x, int32_t y, uint32_t color)
{
    uint32_t *extended_framebuffer = (uint32_t *)framebuffer;

    extended_framebuffer[y * WINDOW_WIDTH + x] = color;
}

void DrawStraightLaine(uint8_t *framebuffer, int32_t start_x, int32_t end_x, int32_t y, uint32_t color)
{
    for (int32_t counter = start_x; counter < end_x; counter++)
    {
        DrawPixel(framebuffer, counter, y, color);
    }
}

int32_t main(int32_t argument_count, char **arguments)
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("SDL2Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH, WINDOW_HEIGHT);
    uint8_t *pixels = new uint8_t[WINDOW_WIDTH * WINDOW_HEIGHT * 4];

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

        DrawStraightLaine(pixels, 10, 200, 10, 0xFFFF0000);

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
