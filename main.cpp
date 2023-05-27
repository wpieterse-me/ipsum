#include <cstdint>
#include <fstream>
#include <chrono>
#include <thread>

#include "perfetto.h"

#include <SDL2/SDL.h>

static constexpr int32_t WINDOW_WIDTH = 640;
static constexpr int32_t WINDOW_HEIGHT = 480;

PERFETTO_DEFINE_CATEGORIES(perfetto::Category("rendering").SetDescription("Events from the graphics subsystem"));

PERFETTO_TRACK_EVENT_STATIC_STORAGE();

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

void InitializePerfetto()
{
    perfetto::TracingInitArgs args;
    // The backends determine where trace events are recorded. For this example we
    // are going to use the in-process tracing service, which only includes in-app
    // events.
    args.backends = perfetto::kInProcessBackend;
    perfetto::Tracing::Initialize(args);
    perfetto::TrackEvent::Register();
}

std::unique_ptr<perfetto::TracingSession> StartTracing()
{
    // The trace config defines which types of data sources are enabled for
    // recording. In this example we just need the "track_event" data source,
    // which corresponds to the TRACE_EVENT trace points.
    perfetto::TraceConfig cfg;
    cfg.add_buffers()->set_size_kb(1024);
    auto *ds_cfg = cfg.add_data_sources()->mutable_config();
    ds_cfg->set_name("track_event");
    auto tracing_session = perfetto::Tracing::NewTrace();
    tracing_session->Setup(cfg);
    tracing_session->StartBlocking();
    return tracing_session;
}

void StopTracing(std::unique_ptr<perfetto::TracingSession> tracing_session)
{
    // Make sure the last event is closed for this example.
    perfetto::TrackEvent::Flush();
    // Stop tracing and read the trace data.
    tracing_session->StopBlocking();
    std::vector<char> trace_data(tracing_session->ReadTraceBlocking());
    // Write the result into a file.
    // Note: To save memory with longer traces, you can tell Perfetto to write
    // directly into a file by passing a file descriptor into Setup() above.
    std::ofstream output;
    output.open("example.pftrace", std::ios::out | std::ios::binary);
    output.write(&trace_data[0], std::streamsize(trace_data.size()));
    output.close();
    PERFETTO_LOG(
        "Trace written in example.pftrace file. To read this trace in "
        "text form, run `./tools/traceconv text example.pftrace`");
}

int32_t main(int32_t argument_count, char **arguments)
{
    InitializePerfetto();
    auto tracing_session = StartTracing();
    // Give a custom name for the traced process.
    perfetto::ProcessTrack process_track = perfetto::ProcessTrack::Current();
    perfetto::protos::gen::TrackDescriptor desc = process_track.Serialize();
    desc.mutable_process()->set_process_name("Example");
    perfetto::TrackEvent::SetTrackDescriptor(process_track, desc);

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("SDL2Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH, WINDOW_HEIGHT);
    uint8_t *pixels = new uint8_t[WINDOW_WIDTH * WINDOW_HEIGHT * 4];

    Point2D a{WINDOW_WIDTH - 10, WINDOW_HEIGHT - 10};
    Point2D b{10, WINDOW_HEIGHT - 10};
    Point2D c{10, 10};

    int32_t fps = 0;
    int32_t fpsTimer = 0;
    char fpsText[256] = {0};

    bool running = true;
    while (running)
    {
        TRACE_EVENT("rendering", "Loop");

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

        int32_t now = SDL_GetTicks();
        if (now > (fpsTimer + 1000))
        {
            sprintf(fpsText, "%d FPS", fps);
            printf("%s\n", fpsText);
            fpsTimer = now;
            fps = 0;
        }

        fps++;
    }

    delete[] pixels;
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    StopTracing(std::move(tracing_session));
    return 0;
}
