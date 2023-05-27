#include <cstdint>
#include <fstream>

#include "third_party/perfetto/perfetto.h"

#include "triangle.h"

PERFETTO_DEFINE_CATEGORIES(perfetto::Category("rendering").SetDescription("Events from the graphics subsystem"));

PERFETTO_TRACK_EVENT_STATIC_STORAGE();

void initialize_perfetto()
{
    perfetto::TracingInitArgs arguments;

    arguments.backends = perfetto::kInProcessBackend;

    perfetto::Tracing::Initialize(arguments);
    perfetto::TrackEvent::Register();
}

std::unique_ptr<perfetto::TracingSession> start_tracing()
{
    perfetto::TraceConfig configuration;

    configuration.add_buffers()->set_size_kb(1024);

    auto *datasource_configuration = configuration.add_data_sources()->mutable_config();

    datasource_configuration->set_name("track_event");

    auto tracing_session = perfetto::Tracing::NewTrace();

    tracing_session->Setup(configuration);
    tracing_session->StartBlocking();

    return tracing_session;
}

void stop_tracing(std::unique_ptr<perfetto::TracingSession> tracing_session)
{
    perfetto::TrackEvent::Flush();
    tracing_session->StopBlocking();

    std::vector<char> trace_data(tracing_session->ReadTraceBlocking());

    std::ofstream output;

    output.open("example.pftrace", std::ios::out | std::ios::binary);
    output.write(&trace_data[0], std::streamsize(trace_data.size()));
    output.close();

    PERFETTO_LOG("Trace written in example.pftrace file. To read this trace in text form, run `./tools/traceconv text example.pftrace`");
}

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
    static constexpr int32_t IMAGE_WIDTH = 8192;
    static constexpr int32_t IMAGE_HEIGHT = 8192;

    point2d_t v0{IMAGE_WIDTH, IMAGE_HEIGHT};
    point2d_t v1{0, IMAGE_HEIGHT};
    point2d_t v2{0, 0};
    uint32_t color = pack_color(255, 0, 0);

    uint32_t *image = new uint32_t[IMAGE_WIDTH * IMAGE_HEIGHT];

    initialize_perfetto();

    auto tracing_session = start_tracing();

    perfetto::ProcessTrack process_track = perfetto::ProcessTrack::Current();

    perfetto::protos::gen::TrackDescriptor desc = process_track.Serialize();

    desc.mutable_process()->set_process_name("Example");

    perfetto::TrackEvent::SetTrackDescriptor(process_track, desc);

    clear_image(image, IMAGE_WIDTH, IMAGE_HEIGHT);

    {
        TRACE_EVENT("rendering", "general");
        draw_triangle_general(image, IMAGE_WIDTH, IMAGE_HEIGHT, v0, v1, v2, color);
    }

    write_framebuffer("out_general.ppm", image, IMAGE_WIDTH, IMAGE_HEIGHT);

    clear_image(image, IMAGE_WIDTH, IMAGE_HEIGHT);

    {
        TRACE_EVENT("rendering", "optimized_1");
        draw_triangle_optimized_1(image, IMAGE_WIDTH, IMAGE_HEIGHT, v0, v1, v2, color);
    }

    write_framebuffer("out_optimized_1.ppm", image, IMAGE_WIDTH, IMAGE_HEIGHT);

    clear_image(image, IMAGE_WIDTH, IMAGE_HEIGHT);

    {
        TRACE_EVENT("rendering", "optimized_2");
        draw_triangle_optimized_2(image, IMAGE_WIDTH, IMAGE_HEIGHT, v0, v1, v2, color);
    }

    write_framebuffer("out_optimized_2.ppm", image, IMAGE_WIDTH, IMAGE_HEIGHT);

#if defined(__AVX2__)

    clear_image(image, IMAGE_WIDTH, IMAGE_HEIGHT);

    {
        TRACE_EVENT("rendering", "avx2");
        draw_triangle_avx2(image, IMAGE_WIDTH, IMAGE_HEIGHT, v0, v1, v2, color);
    }

    write_framebuffer("out_avx2.ppm", image, IMAGE_WIDTH, IMAGE_HEIGHT);

#endif

    stop_tracing(std::move(tracing_session));

    return 0;
}
