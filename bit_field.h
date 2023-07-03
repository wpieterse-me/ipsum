// SPDX-FileCopyrightText: 2014 Tony Wasserka
// SPDX-FileCopyrightText: 2014 Dolphin Emulator Project
// SPDX-License-Identifier: BSD-3-Clause AND GPL-2.0-or-later

#pragma once

#include <cstddef>
#include <limits>
#include <type_traits>

#include "swap.h"

/*
 * Abstract bitfield class
 *
 * Allows endianness-independent access to
 * individual bitfields within some raw
 * integer value. The assembly generated
 * by this class is identical to the
 * usage of raw bitfields, so it's a
 * perfectly fine replacement.
 *
 * For BitField<X,Y,Z>, X is the distance of
 * the bitfield to the LSB of the
 * raw value, Y is the length in bits of the
 * bitfield. Z is an integer type
 * which determines the sign of the bitfield.
 * Z must have the same size as the
 * raw integer.
 *
 *
 * General usage:
 *

 * * Create a new union with the raw integer value as a member.
 * Then for each
 * bitfield you want to expose, add a BitField member
 * in the union. The
 * template parameters are the bit offset and the number
 * of desired bits.
 *

 * * Changes in the bitfield members will then get reflected in the raw integer

 * * value and vice-versa.
 *
 *
 * Sample usage:
 *
 * union SomeRegister
 * {

 * *     u32 hex;
 *
 *     BitField<0,7,u32> first_seven_bits;     // unsigned

 * *     BitField<7,8,u32> next_eight_bits;      // unsigned
 *
 * BitField<3,15,s32> some_signed_fields;  // signed
 * };
 *
 * This is
 * equivalent to the little-endian specific code:
 *
 * union SomeRegister
 * {

 * *     u32 hex;
 *
 *     struct
 *     {
 *         u32 first_seven_bits :
 * 7;
 *         u32 next_eight_bits : 8;
 *     };
 *     struct
 *     {
 *
 * u32 : 3; // padding
 *         s32 some_signed_fields : 15;
 *     };
 * };

 * *
 *
 * Caveats:
 *
 * 1)
 * BitField provides automatic casting from and to
 * the storage type where
 * appropriate. However, when using non-typesafe
 * functions like printf, an
 * explicit cast must be performed on the BitField
 * object to make sure it gets
 * passed correctly, e.g.:
 * printf("Value: %d",
 * (s32)some_register.some_signed_fields);
 *
 * 2)
 * Not really a caveat, but
 * potentially irritating: This class is used in some
 * packed structures that
 * do not guarantee proper alignment. Therefore we have
 * to use #pragma pack
 * here not to pack the members of the class, but instead
 * to break GCC's
 * assumption that the members of the class are aligned on
 *
 * sizeof(StorageType).
 * TODO(neobrain): Confirm that this is a proper fix and
 * not just masking
 * symptoms.
 */
#pragma pack(1)

template <std::size_t Position,
          std::size_t Bits,
          typename T,
          typename EndianTag = LETag>
struct BitField {
  private:
    // UnderlyingType is T for non-enum types and the underlying type of T if
    // T is an enumeration. Note that T is wrapped within an enable_if in the
    // former case to workaround compile errors which arise when using
    // std::underlying_type<T>::type directly.
    using UnderlyingType =
        typename std::conditional_t<std::is_enum_v<T>,
                                    std::underlying_type<T>,
                                    std::enable_if<true, T>>::type;

    // We store the value as the unsigned type to avoid undefined behaviour on
    // value shifting
    using StorageType = std::make_unsigned_t<UnderlyingType>;

    using StorageTypeWithEndian =
        typename AddEndian<StorageType, EndianTag>::type;

  public:
    /// Constants to allow limited introspection of fields if needed
    static constexpr std::size_t position = Position;
    static constexpr std::size_t bits     = Bits;
    static constexpr StorageType mask =
        (((StorageType)~0) >> (8 * sizeof(T) - bits)) << position;

    /**
     * Formats a value by masking and shifting it according to the field
     * parameters. A value
     * containing several bitfields can be assembled
     * by formatting each of their values and ORing
     * the results
     * together.
     */
    [[nodiscard]] static constexpr StorageType FormatValue(const T& value) {
      return (static_cast<StorageType>(value) << position) & mask;
    }

    /**
     * Extracts a value from the passed storage. In most situations
     * prefer use the member functions
     * (such as Value() or operator T),
     * but this can be used to extract a value from a bitfield
     * union in a
     * constexpr context.
     */
    [[nodiscard]] static constexpr T ExtractValue(const StorageType& storage) {
      if constexpr(std::numeric_limits<UnderlyingType>::is_signed) {
        std::size_t shift = 8 * sizeof(T) - bits;
        return static_cast<T>(
            static_cast<UnderlyingType>(storage << (shift - position)) >>
            shift);
      } else {
        return static_cast<T>((storage & mask) >> position);
      }
    }

    // This constructor and assignment operator might be considered ambiguous:
    // Would they initialize the storage or just the bitfield?
    // Hence, delete them. Use the Assign method to set bitfield values!
    BitField(T val)            = delete;
    BitField& operator=(T val) = delete;

    constexpr BitField() noexcept = default;

    constexpr BitField(const BitField&) noexcept            = default;
    constexpr BitField& operator=(const BitField&) noexcept = default;

    constexpr BitField(BitField&&) noexcept            = default;
    constexpr BitField& operator=(BitField&&) noexcept = default;

    constexpr void Assign(const T& value) {
#ifdef _MSC_VER
      storage =
          static_cast<StorageType>((storage & ~mask) | FormatValue(value));
#else
      // Explicitly reload with memcpy to avoid compiler aliasing quirks
      // regarding optimization: GCC/Clang clobber chained stores to
      // different bitfields in the same struct with the last value.
      StorageTypeWithEndian storage_;
      std::memcpy(&storage_, &storage, sizeof(storage_));
      storage =
          static_cast<StorageType>((storage_ & ~mask) | FormatValue(value));
#endif
    }

    [[nodiscard]] constexpr T Value() const {
      return ExtractValue(storage);
    }

    template <typename ConvertedToType>
    [[nodiscard]] constexpr ConvertedToType As() const {
      static_assert(!std::is_same_v<T, ConvertedToType>,
                    "Unnecessary cast. Use Value() instead.");
      return static_cast<ConvertedToType>(Value());
    }

    [[nodiscard]] constexpr operator T() const {
      return Value();
    }

    [[nodiscard]] constexpr explicit operator bool() const {
      return Value() != 0;
    }

  private:
    StorageTypeWithEndian storage;

    static_assert(bits + position <= 8 * sizeof(T), "Bitfield out of range");

    // And, you know, just in case people specify something stupid like
    // bits=position=0x80000000
    static_assert(position < 8 * sizeof(T), "Invalid position");
    static_assert(bits <= 8 * sizeof(T), "Invalid number of bits");
    static_assert(bits > 0, "Invalid number of bits");
    static_assert(std::is_trivially_copyable_v<T>,
                  "T must be trivially copyable in a BitField");
};

#pragma pack()

template <std::size_t Position, std::size_t Bits, typename T>
using BitFieldBE = BitField<Position, Bits, T, BETag>;

template <std::size_t Position,
          std::size_t Bits,
          typename T,
          typename EndianTag = LETag>
inline auto format_as(BitField<Position, Bits, T, EndianTag> bitfield) {
  return bitfield.Value();
}
