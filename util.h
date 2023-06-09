#pragma once

#include <stdint.h>

#define SIGNATURE_8(ValueA) (ValueA)

#define SIGNATURE_16(ValueA, ValueB)                                           \
  (SIGNATURE_8(ValueA) | (SIGNATURE_8(ValueB) << 8))

#define SIGNATURE_32(ValueA, ValueB, ValueC, ValueD)                           \
  (SIGNATURE_16(ValueA, ValueB) | (SIGNATURE_16(ValueC, ValueD) << 16))

#define OFFSET_OF(Type, Field) ((size_t) __builtin_offsetof(Type, Field))

#define BASE_CONTAINER_OF(Record, Type, Field)                                 \
  ((Type*)((uint8_t*)(Record)-OFFSET_OF(Type, Field)))

#define CONTAINER_OF(Record, Type, Field, TestSignature)                       \
  BASE_CONTAINER_OF(Record, Type, Field)

#if defined(__cplusplus)
extern "C" {
#endif

  uint32_t pack_color(const uint8_t red,
                      const uint8_t green,
                      const uint8_t blue,
                      const uint8_t alpha);

  void unpack_color(const uint32_t color,
                    uint8_t*       r,
                    uint8_t*       g,
                    uint8_t*       b,
                    uint8_t*       a);

  void clear_framebuffer(uint32_t* image, int32_t image_width, int32_t image_height);

  void write_framebuffer(const char*     filename,
                         const uint32_t* image,
                         const int32_t   image_width,
                         const int32_t   image_height);

#if defined(__cplusplus)
}
#endif
