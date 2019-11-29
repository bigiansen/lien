#pragma once

#include <cinttypes>
#include <type_traits>

namespace ien
{
    template<typename T>
    constexpr void LIEN_RESTRICT_INTEGRAL() 
    { static_assert(std::is_integral<T>::value, "Not an integral type!"); }

    template<typename T, size_t sz>
    constexpr void LIEN_RESTRICT_SIZE() 
    { static_assert(sizeof(T) == sz, "sizeof(TInt8) differs from specified size!"); }

    template<typename TInt>
    bool get_bit(TInt v, size_t i)
    {
        LIEN_RESTRICT_INTEGRAL<TInt>();
        TInt mask = static_cast<TInt>(1) << i;
        return (v | mask) == v;
    }

    template<typename TInt>
    void set_bit(TInt& v, size_t i)
    {
        LIEN_RESTRICT_INTEGRAL<TInt>();
        v |= static_cast<TInt>(1) << i;
    }

    template<typename TInt>
    void unset_bit(TInt& v, size_t i)
    {
        LIEN_RESTRICT_INTEGRAL<TInt>();
        v &= ~(static_cast<TInt>(1) << i);
    }

    template<typename TInt8>
    constexpr TInt8 lo_nibble(TInt8 v)
    {
        LIEN_RESTRICT_INTEGRAL<TInt8>();
        LIEN_RESTRICT_SIZE<TInt8, 1>();
        return static_cast<TInt8>(v & 0x0F);
    }

    template<typename TInt8>
    constexpr TInt8 hi_nibble(TInt8 v)
    {
        LIEN_RESTRICT_INTEGRAL<TInt8>();
        LIEN_RESTRICT_SIZE<TInt8, 1>();
        return (v & static_cast<TInt8>(0xF0)) >> 4;
    }

    template<typename TInt16, typename TInt8 = uint8_t>
    constexpr TInt8 lo_byte(TInt16 v)
    {
        LIEN_RESTRICT_INTEGRAL<TInt16>();
        LIEN_RESTRICT_INTEGRAL<TInt8>();
        LIEN_RESTRICT_SIZE<TInt16, 2>();
        LIEN_RESTRICT_SIZE<TInt8, 1>();
        return static_cast<TInt8>(v & 0x00FF);
    }

    template<typename TInt16, typename TInt8 = uint8_t>
    constexpr TInt8 hi_byte(TInt16 v)
    {
        LIEN_RESTRICT_INTEGRAL<TInt16>();
        LIEN_RESTRICT_INTEGRAL<TInt8>();
        LIEN_RESTRICT_SIZE<TInt16, 2>();
        LIEN_RESTRICT_SIZE<TInt8, 1>();
        return static_cast<TInt8>((v & 0xFF00) >> 8);
    }

    template<typename TInt32, typename TInt16 = uint16_t>
    constexpr TInt16 lo_word(TInt32 v)
    {
        LIEN_RESTRICT_INTEGRAL<TInt32>();
        LIEN_RESTRICT_INTEGRAL<TInt16>();
        LIEN_RESTRICT_SIZE<TInt32, 4>();
        LIEN_RESTRICT_SIZE<TInt16, 2>();
        return static_cast<TInt16>(v & (0x0000FFFF));
    }

    template<typename TInt32, typename TInt16 = uint16_t>
    constexpr TInt16 hi_word(TInt32 v)
    {
        LIEN_RESTRICT_INTEGRAL<TInt32>();
        LIEN_RESTRICT_INTEGRAL<TInt16>();
        LIEN_RESTRICT_SIZE<TInt32, 4>();
        LIEN_RESTRICT_SIZE<TInt16, 2>();
        return static_cast<TInt16>((v & 0xFFFF0000) >> 16);
    }

    template<typename TInt64, typename TInt32 = uint32_t>
    constexpr TInt32 lo_dword(TInt64 v)
    {
        LIEN_RESTRICT_INTEGRAL<TInt64>();
        LIEN_RESTRICT_INTEGRAL<TInt32>();
        LIEN_RESTRICT_SIZE<TInt64, 8>();
        LIEN_RESTRICT_SIZE<TInt32, 4>();
        return static_cast<TInt32>(v & 0x00000000FFFFFFFF);
    }

    template<typename TInt64, typename TInt32 = uint32_t>
    constexpr TInt32 hi_dword(TInt64 v)
    {
        LIEN_RESTRICT_INTEGRAL<TInt64>();
        LIEN_RESTRICT_INTEGRAL<TInt32>();
        LIEN_RESTRICT_SIZE<TInt64, 8>();
        LIEN_RESTRICT_SIZE<TInt32, 4>();
        return static_cast<TInt32>((v & 0xFFFFFFFF00000000) >> 32);
    }
}