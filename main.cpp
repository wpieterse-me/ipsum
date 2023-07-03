#include <array>
#include <vector>
#include <functional>
#include <iostream>
#include <span>

#include "util.h"

struct color_3_f {
    float r;
    float g;
    float b;
};

void generate_random_pattern() {
  uint32_t                 image_width  = 512;
  uint32_t                 image_height = 512;
  std::array<color_3_f, 3> rock_colors;

  rock_colors[0] = { 0.4078f, 0.4078f, 0.3764f };
  rock_colors[1] = { 0.7606f, 0.6274f, 0.6313f };
  rock_colors[2] = { 0.8980f, 0.9372f, 0.9725f };

  std::vector<uint32_t> framebuffer{};

  framebuffer.resize(image_width * image_height);

  for(uint32_t x = 0; x < image_width; x++) {
    for(uint32_t y = 0; y < image_height; y++) {
      size_t color_index = std::min(size_t(drand48() * rock_colors.size()),
                                    rock_colors.size() - 1);

      uint32_t r = rock_colors[color_index].r * 255;
      uint32_t g = rock_colors[color_index].g * 255;
      uint32_t b = rock_colors[color_index].b * 255;
      uint32_t c = pack_color(r, g, b, 255);

      framebuffer[x + y * image_width] = c;
    }
  }

  write_framebuffer("random_pattern.ppm",
                    framebuffer.data(),
                    image_width,
                    image_height);
}

struct noise_operator {
    float table;

    noise_operator(float value)
      : table{ value } {
    }

    float operator()() const {
      return table;
    }
};

struct smoothing_operator {
    template <class NoiseOperator>
    float operator()(NoiseOperator& noise) const {
      return noise() * 2.0f;
    }
};

void do_thing() {
  noise_operator     noise{ 8.0f };
  smoothing_operator smoothing{};

  auto generate = std::bind(smoothing, noise);

  std::cout << generate() << std::endl;
}

int32_t main(int32_t argument_count, char** arguments) {
  return 0;
}
