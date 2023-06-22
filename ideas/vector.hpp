#pragma once

#include <cstdint>

#include <immintrin.h>

#include <array>

template <typename ElementType, size_t Dimensions>
struct VectorSIMD {
    using Type = std::array<ElementType, Dimensions>;
};

template <>
struct VectorSIMD<float, 1> {
    using Type = __m128;
};

template <>
struct VectorSIMD<float, 2> {
    using Type = __m128;
};

template <>
struct VectorSIMD<float, 3> {
    using Type = __m128;
};

template <>
struct VectorSIMD<float, 4> {
    using Type = __m128;
};

template <>
struct VectorSIMD<float, 5> {
    using Type = __m256;
};

template <>
struct VectorSIMD<float, 6> {
    using Type = __m256;
};

template <>
struct VectorSIMD<float, 7> {
    using Type = __m256;
};

template <>
struct VectorSIMD<float, 8> {
    using Type = __m256;
};

template <typename ElementType, size_t Dimensions>
struct Vector {
    using StandardStorage = std::array<ElementType, Dimensions>;
    using SIMDStorage     = VectorSIMD<ElementType, Dimensions>::Type;

    union {
        StandardStorage standard_storage;
        SIMDStorage     simd_storage;
    };
};

template <typename ElementType, size_t Dimensions>
void initialize(Vector<ElementType, Dimensions>& item) {
  if constexpr(Dimensions > 0 && Dimensions <= 4) {
    item.simd_storage = _mm_setzero_ps();

    return;
  }

  if constexpr(Dimensions > 4 && Dimensions <= 8) {
    item.simd_storage = _mm256_setzero_ps();

    return;
  }

  for(size_t index = 0; index < Dimensions; index++) {
    item.standard_storage[index] = ElementType{};
  }
}

template <typename ElementType, size_t Dimensions>
Vector<ElementType, Dimensions>
operator+(const Vector<ElementType, Dimensions>& lhs,
          const Vector<ElementType, Dimensions>& rhs) {
  Vector<ElementType, Dimensions> result;

  for(size_t index = 0; index < Dimensions; index++) {
    auto lhs_value    = lhs.standard_storage[index];
    auto rhs_value    = rhs.standard_storage[index];
    auto result_value = lhs_value + rhs_value;

    result.standard_storage[index] = result_value;
  }

  return result;
}

template <>
Vector<float, 4> operator+(const Vector<float, 4>& lhs,
                           const Vector<float, 4>& rhs) {
  Vector<float, 4> result;

  const auto lhs_value = lhs.simd_storage;
  const auto rhs_value = rhs.simd_storage;

  result.simd_storage = _mm_add_ps(lhs_value, rhs_value);

  return result;
}

using dec_32 = float;
using dec_64 = double;

using Vector_D32_1 = Vector<dec_32, 1>;
using Vector_D32_2 = Vector<dec_32, 2>;
using Vector_D32_3 = Vector<dec_32, 3>;
using Vector_D32_4 = Vector<dec_32, 4>;
using Vector_D32_5 = Vector<dec_32, 5>;
using Vector_D32_6 = Vector<dec_32, 6>;
using Vector_D32_7 = Vector<dec_32, 7>;
using Vector_D32_8 = Vector<dec_32, 8>;

using Vector_D64_1 = Vector<dec_64, 1>;
using Vector_D64_2 = Vector<dec_64, 2>;
using Vector_D64_3 = Vector<dec_64, 3>;
using Vector_D64_4 = Vector<dec_64, 4>;
using Vector_D64_5 = Vector<dec_64, 5>;
using Vector_D64_6 = Vector<dec_64, 6>;
using Vector_D64_7 = Vector<dec_64, 7>;
using Vector_D64_8 = Vector<dec_64, 8>;
