#include "util.h"

#include <fstream>

uint32_t pack_color(const uint8_t red,
                    const uint8_t green,
                    const uint8_t blue,
                    const uint8_t alpha) {
  return (alpha << 24) + (blue << 16) + (green << 8) + red;
}

void unpack_color(const uint32_t color,
                  uint8_t*       r,
                  uint8_t*       g,
                  uint8_t*       b,
                  uint8_t*       a) {
  *r = (color >> 0) * 255;
  *g = (color >> 8) * 255;
  *b = (color >> 16) * 255;
  *a = (color >> 24) * 255;
}

void clear_framebuffer(uint32_t* image, int32_t image_width, int32_t image_height) {
  for(int32_t index_y = 0; index_y < image_height; index_y++) {
    for(int32_t index_x = 0; index_x < image_width; index_x++) {
      image[index_x + index_y * image_width] = pack_color(0, 0, 0, 255);
    }
  }
}

void write_framebuffer(const char*     filename,
                       const uint32_t* image,
                       const int32_t   image_width,
                       const int32_t   image_height) {
  std::ofstream ofs(filename, std::ios::binary);
  ofs << "P6" << std::endl
      << image_width << " " << image_height << std::endl
      << "255" << std::endl;

  for(int32_t index = 0; index < (image_width * image_height); index++) {
    uint8_t red   = 0;
    uint8_t green = 0;
    uint8_t blue  = 0;
    uint8_t alpha = 0;

    unpack_color(image[index], &red, &green, &blue, &alpha);

    ofs << static_cast<char>(red) << static_cast<char>(green)
        << static_cast<char>(blue);
  }

  ofs.close();
}
