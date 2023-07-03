#include <cstdint>

#include "util.h"
#include "graphics/rasterizer/triangle.h"

int32_t main(int32_t argument_count, char** arguments) {
  static constexpr int32_t IMAGE_WIDTH  = 512;
  static constexpr int32_t IMAGE_HEIGHT = 512;

  point2d_t v0{ IMAGE_WIDTH, IMAGE_HEIGHT, 1.0f, 0.0f, 0.0f };
  point2d_t v1{ 0, IMAGE_HEIGHT, 0.0f, 1.0f, 0.0f };
  point2d_t v2{ 0, 0, 0.0f, 0.5f, 1.0f };

  uint32_t* image = new uint32_t[IMAGE_WIDTH * IMAGE_HEIGHT];

  /*
  clear_framebuffer(image, IMAGE_WIDTH, IMAGE_HEIGHT);
  draw_triangle_general(image, IMAGE_WIDTH, IMAGE_HEIGHT, v0, v1, v2, color);
  write_framebuffer("out_general.ppm", image, IMAGE_WIDTH, IMAGE_HEIGHT);

  clear_framebuffer(image, IMAGE_WIDTH, IMAGE_HEIGHT);
  draw_triangle_optimized_1(image, IMAGE_WIDTH, IMAGE_HEIGHT, v0, v1, v2,
  color); write_framebuffer("out_optimized_1.ppm", image, IMAGE_WIDTH,
  IMAGE_HEIGHT);

  clear_framebuffer(image, IMAGE_WIDTH, IMAGE_HEIGHT);
  draw_triangle_optimized_2(image, IMAGE_WIDTH, IMAGE_HEIGHT, v0, v1, v2,
  color); write_framebuffer("out_optimized_2.ppm", image, IMAGE_WIDTH,
  IMAGE_HEIGHT);
  */

  clear_framebuffer(image, IMAGE_WIDTH, IMAGE_HEIGHT);
  draw_triangle_joshbeam(image, IMAGE_WIDTH, IMAGE_HEIGHT, v0, v1, v2);
  write_framebuffer("out_joshbeam.ppm", image, IMAGE_WIDTH, IMAGE_HEIGHT);

  clear_framebuffer(image, IMAGE_WIDTH, IMAGE_HEIGHT);
  draw_triangle_trenki2_p1(image, IMAGE_WIDTH, IMAGE_HEIGHT, v0, v1, v2);
  write_framebuffer("out_trenki2_p1.ppm", image, IMAGE_WIDTH, IMAGE_HEIGHT);

  /*
  #if defined(__AVX2__)

      clear_framebuffer(image, IMAGE_WIDTH, IMAGE_HEIGHT);
      draw_triangle_avx2(image, IMAGE_WIDTH, IMAGE_HEIGHT, v0, v1, v2, color);
      write_framebuffer("out_avx2.ppm", image, IMAGE_WIDTH, IMAGE_HEIGHT);

  #endif
  */

  return 0;
}
