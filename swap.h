// SPDX-FileCopyrightText: 2012 PPSSPP Project
// SPDX-FileCopyrightText: 2012 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

// Official git repository and contact information can be found at
// https://github.com/hrydgard/ppsspp and http://www.ppsspp.org/.

#pragma once

#if defined(_MSC_VER)
# include <cstdlib>
#endif
#include <bit>
#include <cstring>
#include <type_traits>

#include <cstdint>

namespace Common {

#ifdef _MSC_VER
  [[nodiscard]] inline uint16_t swap16(uint16_t data) noexcept {
    return _byteswap_ushort(data);
  }

  [[nodiscard]] inline uint32_t swap32(uint32_t data) noexcept {
    return _byteswap_ulong(data);
  }

  [[nodiscard]] inline uint64_t swap64(uint64_t data) noexcept {
    return _byteswap_uint64(data);
  }
#elif defined(__clang__) || defined(__GNUC__)
# if defined(__Bitrig__) || defined(__OpenBSD__)
// redefine swap16, swap32, swap64 as inline functions
#  undef swap16
#  undef swap32
#  undef swap64
# endif
  [[nodiscard]] inline uint16_t swap16(uint16_t data) noexcept {
    return __builtin_bswap16(data);
  }

  [[nodiscard]] inline uint32_t swap32(uint32_t data) noexcept {
    return __builtin_bswap32(data);
  }

  [[nodiscard]] inline uint64_t swap64(uint64_t data) noexcept {
    return __builtin_bswap64(data);
  }
#else
  // Generic implementation.
  [[nodiscard]] inline uint16_t swap16(uint16_t data) noexcept {
    return (data >> 8) | (data << 8);
  }

  [[nodiscard]] inline uint32_t swap32(uint32_t data) noexcept {
    return ((data & 0xFF000000U) >> 24) | ((data & 0x00FF0000U) >> 8) |
           ((data & 0x0000FF00U) << 8) | ((data & 0x000000FFU) << 24);
  }

  [[nodiscard]] inline uint64_t swap64(uint64_t data) noexcept {
    return ((data & 0xFF00000000000000ULL) >> 56) |
           ((data & 0x00FF000000000000ULL) >> 40) |
           ((data & 0x0000FF0000000000ULL) >> 24) |
           ((data & 0x000000FF00000000ULL) >> 8) |
           ((data & 0x00000000FF000000ULL) << 8) |
           ((data & 0x0000000000FF0000ULL) << 24) |
           ((data & 0x000000000000FF00ULL) << 40) |
           ((data & 0x00000000000000FFULL) << 56);
  }
#endif

  [[nodiscard]] inline float swapf(float f) noexcept {
    static_assert(sizeof(uint32_t) == sizeof(float),
                  "float must be the same size as uint32_t.");

    uint32_t value;
    std::memcpy(&value, &f, sizeof(uint32_t));

    value = swap32(value);
    std::memcpy(&f, &value, sizeof(uint32_t));

    return f;
  }

  [[nodiscard]] inline double swapd(double f) noexcept {
    static_assert(sizeof(uint64_t) == sizeof(double),
                  "double must be the same size as uint64_t.");

    uint64_t value;
    std::memcpy(&value, &f, sizeof(uint64_t));

    value = swap64(value);
    std::memcpy(&f, &value, sizeof(uint64_t));

    return f;
  }

} // Namespace Common

template <typename T, typename F>
struct swap_struct_t {
    using swapped_t = swap_struct_t;

  protected:
    T value;

    static T swap(T v) {
      return F::swap(v);
    }

  public:
    T swap() const {
      return swap(value);
    }

    swap_struct_t() = default;

    swap_struct_t(const T& v)
      : value(swap(v)) {
    }

    template <typename S>
    swapped_t& operator=(const S& source) {
      value = swap(static_cast<T>(source));
      return *this;
    }

    operator int8_t() const {
      return static_cast<int8_t>(swap());
    }

    operator uint8_t() const {
      return static_cast<uint8_t>(swap());
    }

    operator int16_t() const {
      return static_cast<int16_t>(swap());
    }

    operator uint16_t() const {
      return static_cast<uint16_t>(swap());
    }

    operator int32_t() const {
      return static_cast<int32_t>(swap());
    }

    operator uint32_t() const {
      return static_cast<uint32_t>(swap());
    }

    operator int64_t() const {
      return static_cast<int64_t>(swap());
    }

    operator uint64_t() const {
      return static_cast<uint64_t>(swap());
    }

    operator float() const {
      return static_cast<float>(swap());
    }

    operator double() const {
      return static_cast<double>(swap());
    }

    // +v
    swapped_t operator+() const {
      return +swap();
    }

    // -v
    swapped_t operator-() const {
      return -swap();
    }

    // v / 5
    swapped_t operator/(const swapped_t& i) const {
      return swap() / i.swap();
    }

    template <typename S>
    swapped_t operator/(const S& i) const {
      return swap() / i;
    }

    // v * 5
    swapped_t operator*(const swapped_t& i) const {
      return swap() * i.swap();
    }

    template <typename S>
    swapped_t operator*(const S& i) const {
      return swap() * i;
    }

    // v + 5
    swapped_t operator+(const swapped_t& i) const {
      return swap() + i.swap();
    }

    template <typename S>
    swapped_t operator+(const S& i) const {
      return swap() + static_cast<T>(i);
    }

    // v - 5
    swapped_t operator-(const swapped_t& i) const {
      return swap() - i.swap();
    }

    template <typename S>
    swapped_t operator-(const S& i) const {
      return swap() - static_cast<T>(i);
    }

    // v += 5
    swapped_t& operator+=(const swapped_t& i) {
      value = swap(swap() + i.swap());
      return *this;
    }

    template <typename S>
    swapped_t& operator+=(const S& i) {
      value = swap(swap() + static_cast<T>(i));
      return *this;
    }

    // v -= 5
    swapped_t& operator-=(const swapped_t& i) {
      value = swap(swap() - i.swap());
      return *this;
    }

    template <typename S>
    swapped_t& operator-=(const S& i) {
      value = swap(swap() - static_cast<T>(i));
      return *this;
    }

    // ++v
    swapped_t& operator++() {
      value = swap(swap() + 1);
      return *this;
    }

    // --v
    swapped_t& operator--() {
      value = swap(swap() - 1);
      return *this;
    }

    // v++
    swapped_t operator++(int) {
      swapped_t old = *this;
      value         = swap(swap() + 1);
      return old;
    }

    // v--
    swapped_t operator--(int) {
      swapped_t old = *this;
      value         = swap(swap() - 1);
      return old;
    }

    // Comparison
    // v == i
    bool operator==(const swapped_t& i) const {
      return swap() == i.swap();
    }

    template <typename S>
    bool operator==(const S& i) const {
      return swap() == i;
    }

    // v != i
    bool operator!=(const swapped_t& i) const {
      return swap() != i.swap();
    }

    template <typename S>
    bool operator!=(const S& i) const {
      return swap() != i;
    }

    // v > i
    bool operator>(const swapped_t& i) const {
      return swap() > i.swap();
    }

    template <typename S>
    bool operator>(const S& i) const {
      return swap() > i;
    }

    // v < i
    bool operator<(const swapped_t& i) const {
      return swap() < i.swap();
    }

    template <typename S>
    bool operator<(const S& i) const {
      return swap() < i;
    }

    // v >= i
    bool operator>=(const swapped_t& i) const {
      return swap() >= i.swap();
    }

    template <typename S>
    bool operator>=(const S& i) const {
      return swap() >= i;
    }

    // v <= i
    bool operator<=(const swapped_t& i) const {
      return swap() <= i.swap();
    }

    template <typename S>
    bool operator<=(const S& i) const {
      return swap() <= i;
    }

    // logical
    swapped_t operator!() const {
      return !swap();
    }

    // bitmath
    swapped_t operator~() const {
      return ~swap();
    }

    swapped_t operator&(const swapped_t& b) const {
      return swap() & b.swap();
    }

    template <typename S>
    swapped_t operator&(const S& b) const {
      return swap() & b;
    }

    swapped_t& operator&=(const swapped_t& b) {
      value = swap(swap() & b.swap());
      return *this;
    }

    template <typename S>
    swapped_t& operator&=(const S b) {
      value = swap(swap() & b);
      return *this;
    }

    swapped_t operator|(const swapped_t& b) const {
      return swap() | b.swap();
    }

    template <typename S>
    swapped_t operator|(const S& b) const {
      return swap() | b;
    }

    swapped_t& operator|=(const swapped_t& b) {
      value = swap(swap() | b.swap());
      return *this;
    }

    template <typename S>
    swapped_t& operator|=(const S& b) {
      value = swap(swap() | b);
      return *this;
    }

    swapped_t operator^(const swapped_t& b) const {
      return swap() ^ b.swap();
    }

    template <typename S>
    swapped_t operator^(const S& b) const {
      return swap() ^ b;
    }

    swapped_t& operator^=(const swapped_t& b) {
      value = swap(swap() ^ b.swap());
      return *this;
    }

    template <typename S>
    swapped_t& operator^=(const S& b) {
      value = swap(swap() ^ b);
      return *this;
    }

    template <typename S>
    swapped_t operator<<(const S& b) const {
      return swap() << b;
    }

    template <typename S>
    swapped_t& operator<<=(const S& b) const {
      value = swap(swap() << b);
      return *this;
    }

    template <typename S>
    swapped_t operator>>(const S& b) const {
      return swap() >> b;
    }

    template <typename S>
    swapped_t& operator>>=(const S& b) const {
      value = swap(swap() >> b);
      return *this;
    }

    // Member
    /** todo **/

    // Arithmetic
    template <typename S, typename T2, typename F2>
    friend S operator+(const S& p, const swapped_t v);

    template <typename S, typename T2, typename F2>
    friend S operator-(const S& p, const swapped_t v);

    template <typename S, typename T2, typename F2>
    friend S operator/(const S& p, const swapped_t v);

    template <typename S, typename T2, typename F2>
    friend S operator*(const S& p, const swapped_t v);

    template <typename S, typename T2, typename F2>
    friend S operator%(const S& p, const swapped_t v);

    // Arithmetic + assignments
    template <typename S, typename T2, typename F2>
    friend S operator+=(const S& p, const swapped_t v);

    template <typename S, typename T2, typename F2>
    friend S operator-=(const S& p, const swapped_t v);

    // Bitmath
    template <typename S, typename T2, typename F2>
    friend S operator&(const S& p, const swapped_t v);

    // Comparison
    template <typename S, typename T2, typename F2>
    friend bool operator<(const S& p, const swapped_t v);

    template <typename S, typename T2, typename F2>
    friend bool operator>(const S& p, const swapped_t v);

    template <typename S, typename T2, typename F2>
    friend bool operator<=(const S& p, const swapped_t v);

    template <typename S, typename T2, typename F2>
    friend bool operator>=(const S& p, const swapped_t v);

    template <typename S, typename T2, typename F2>
    friend bool operator!=(const S& p, const swapped_t v);

    template <typename S, typename T2, typename F2>
    friend bool operator==(const S& p, const swapped_t v);
};

// Arithmetic
template <typename S, typename T, typename F>
S operator+(const S& i, const swap_struct_t<T, F> v) {
  return i + v.swap();
}

template <typename S, typename T, typename F>
S operator-(const S& i, const swap_struct_t<T, F> v) {
  return i - v.swap();
}

template <typename S, typename T, typename F>
S operator/(const S& i, const swap_struct_t<T, F> v) {
  return i / v.swap();
}

template <typename S, typename T, typename F>
S operator*(const S& i, const swap_struct_t<T, F> v) {
  return i * v.swap();
}

template <typename S, typename T, typename F>
S operator%(const S& i, const swap_struct_t<T, F> v) {
  return i % v.swap();
}

// Arithmetic + assignments
template <typename S, typename T, typename F>
S& operator+=(S& i, const swap_struct_t<T, F> v) {
  i += v.swap();
  return i;
}

template <typename S, typename T, typename F>
S& operator-=(S& i, const swap_struct_t<T, F> v) {
  i -= v.swap();
  return i;
}

// Logical
template <typename S, typename T, typename F>
S operator&(const S& i, const swap_struct_t<T, F> v) {
  return i & v.swap();
}

template <typename S, typename T, typename F>
S operator&(const swap_struct_t<T, F> v, const S& i) {
  return static_cast<S>(v.swap() & i);
}

// Comparison
template <typename S, typename T, typename F>
bool operator<(const S& p, const swap_struct_t<T, F> v) {
  return p < v.swap();
}

template <typename S, typename T, typename F>
bool operator>(const S& p, const swap_struct_t<T, F> v) {
  return p > v.swap();
}

template <typename S, typename T, typename F>
bool operator<=(const S& p, const swap_struct_t<T, F> v) {
  return p <= v.swap();
}

template <typename S, typename T, typename F>
bool operator>=(const S& p, const swap_struct_t<T, F> v) {
  return p >= v.swap();
}

template <typename S, typename T, typename F>
bool operator!=(const S& p, const swap_struct_t<T, F> v) {
  return p != v.swap();
}

template <typename S, typename T, typename F>
bool operator==(const S& p, const swap_struct_t<T, F> v) {
  return p == v.swap();
}

template <typename T>
struct swap_64_t {
    static T swap(T x) {
      return static_cast<T>(Common::swap64(x));
    }
};

template <typename T>
struct swap_32_t {
    static T swap(T x) {
      return static_cast<T>(Common::swap32(x));
    }
};

template <typename T>
struct swap_16_t {
    static T swap(T x) {
      return static_cast<T>(Common::swap16(x));
    }
};

template <typename T>
struct swap_float_t {
    static T swap(T x) {
      return static_cast<T>(Common::swapf(x));
    }
};

template <typename T>
struct swap_double_t {
    static T swap(T x) {
      return static_cast<T>(Common::swapd(x));
    }
};

template <typename T>
struct swap_enum_t {
    static_assert(std::is_enum_v<T>);
    using base = std::underlying_type_t<T>;

  public:
    swap_enum_t() = default;

    swap_enum_t(const T& v)
      : value(swap(v)) {
    }

    swap_enum_t& operator=(const T& v) {
      value = swap(v);
      return *this;
    }

    operator T() const {
      return swap(value);
    }

    explicit operator base() const {
      return static_cast<base>(swap(value));
    }

  protected:
    T value{};
    // clang-format off
    using swap_t = std::conditional_t<
        std::is_same_v<base, uint16_t>, swap_16_t<uint16_t>, std::conditional_t<
        std::is_same_v<base, int16_t>, swap_16_t<int16_t>, std::conditional_t<
        std::is_same_v<base, uint32_t>, swap_32_t<uint32_t>, std::conditional_t<
        std::is_same_v<base, int32_t>, swap_32_t<int32_t>, std::conditional_t<
        std::is_same_v<base, uint64_t>, swap_64_t<uint64_t>, std::conditional_t<
        std::is_same_v<base, int64_t>, swap_64_t<int64_t>, void>>>>>>;

    // clang-format on
    static T swap(T x) {
      return static_cast<T>(swap_t::swap(static_cast<base>(x)));
    }
};

struct SwapTag { }; // Use the different endianness from the system

struct KeepTag { }; // Use the same endianness as the system

template <typename T, typename Tag>
struct AddEndian;

// KeepTag specializations

template <typename T>
struct AddEndian<T, KeepTag> {
    using type = T;
};

// SwapTag specializations

template <>
struct AddEndian<uint8_t, SwapTag> {
    using type = uint8_t;
};

template <>
struct AddEndian<uint16_t, SwapTag> {
    using type = swap_struct_t<uint16_t, swap_16_t<uint16_t>>;
};

template <>
struct AddEndian<uint32_t, SwapTag> {
    using type = swap_struct_t<uint32_t, swap_32_t<uint32_t>>;
};

template <>
struct AddEndian<uint64_t, SwapTag> {
    using type = swap_struct_t<uint64_t, swap_64_t<uint64_t>>;
};

template <>
struct AddEndian<int8_t, SwapTag> {
    using type = int8_t;
};

template <>
struct AddEndian<int16_t, SwapTag> {
    using type = swap_struct_t<int16_t, swap_16_t<int16_t>>;
};

template <>
struct AddEndian<int32_t, SwapTag> {
    using type = swap_struct_t<int32_t, swap_32_t<int32_t>>;
};

template <>
struct AddEndian<int64_t, SwapTag> {
    using type = swap_struct_t<int64_t, swap_64_t<int64_t>>;
};

template <>
struct AddEndian<float, SwapTag> {
    using type = swap_struct_t<float, swap_float_t<float>>;
};

template <>
struct AddEndian<double, SwapTag> {
    using type = swap_struct_t<double, swap_double_t<double>>;
};

template <typename T>
struct AddEndian<T, SwapTag> {
    static_assert(std::is_enum_v<T>);
    using type = swap_enum_t<T>;
};

// Alias LETag/BETag as KeepTag/SwapTag depending on the system
using LETag = std::
    conditional_t<std::endian::native == std::endian::little, KeepTag, SwapTag>;
using BETag = std::
    conditional_t<std::endian::native == std::endian::big, KeepTag, SwapTag>;

// Aliases for LE types
using uint16_t_le = AddEndian<uint16_t, LETag>::type;
using uint32_t_le = AddEndian<uint32_t, LETag>::type;
using uint64_t_le = AddEndian<uint64_t, LETag>::type;

using int16_t_le = AddEndian<int16_t, LETag>::type;
using int32_t_le = AddEndian<int32_t, LETag>::type;
using int64_t_le = AddEndian<int64_t, LETag>::type;

template <typename T>
using enum_le =
    std::enable_if_t<std::is_enum_v<T>, typename AddEndian<T, LETag>::type>;

using float_le  = AddEndian<float, LETag>::type;
using double_le = AddEndian<double, LETag>::type;

// Aliases for BE types
using uint16_t_be = AddEndian<uint16_t, BETag>::type;
using uint32_t_be = AddEndian<uint32_t, BETag>::type;
using uint64_t_be = AddEndian<uint64_t, BETag>::type;

using int16_t_be = AddEndian<int16_t, BETag>::type;
using int32_t_be = AddEndian<int32_t, BETag>::type;
using int64_t_be = AddEndian<int64_t, BETag>::type;

template <typename T>
using enum_be =
    std::enable_if_t<std::is_enum_v<T>, typename AddEndian<T, BETag>::type>;

using float_be  = AddEndian<float, BETag>::type;
using double_be = AddEndian<double, BETag>::type;
