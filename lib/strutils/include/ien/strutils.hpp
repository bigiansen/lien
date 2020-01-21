#pragma once

#include <array>
#include <charconv>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace ien::strutils
{
    extern std::vector<std::string> split(const std::string& str, char delim);
    extern std::vector<std::string> split(std::string_view str, char delim);

    extern std::vector<std::string_view> split_view(const std::string& str, char delim);
    extern std::vector<std::string_view> split_view(std::string_view str, char delim);

    extern bool contains(const std::string& str, char ocurrence);
    extern bool contains(const std::string& str, const std::string& ocurrence);
    extern bool contains(const std::string& str, std::string_view ocurrence);

    extern bool contains(std::string_view str, char ocurrence);
    extern bool contains(std::string_view str, const std::string& ocurrence);
    extern bool contains(std::string_view str, std::string_view ocurrence);

    [[nodiscard]] extern std::string replace(const std::string& str, char ocurrence, char replacement);
    [[nodiscard]] extern std::string replace(std::string_view str, char ocurrence, char replacement);

    [[nodiscard]] extern std::string replace(const std::string& str, char ocurrence, char replacement);
    [[nodiscard]] extern std::string replace(const std::string& str, const std::string& ocurrence, const std::string& replacement);

    extern void replace_in_place(std::string& str, char ocurrence, char replacement);

    [[nodiscard]] extern std::string to_upper(std::string_view str);
    [[nodiscard]] extern std::string to_lower(std::string_view str);
    extern void to_upper_in_place(std::string& str);
    extern void to_lower_in_place(std::string& str);

    [[nodiscard]] extern std::string_view trim_start(std::string_view str);
    [[nodiscard]] extern std::string_view trim_end(std::string_view str);
    [[nodiscard]] extern std::string_view trim(std::string_view str);

    template<typename T>
    [[nodiscard]] T to_integral(std::string_view sv)
    {
        static_assert(std::is_integral_v<T>, "Not an integral type");
        T result;
        char* endptr;

        if constexpr(std::is_signed_v<T>)
        {
            if constexpr(sizeof(T) <= sizeof(long))
                result = static_cast<T>(std::strtol(sv.data(), &endptr, 10));
            else if constexpr(sizeof(T) <= sizeof(long long))
                result = static_cast<T>(std::strtoll(sv.data(), &endptr, 10));
            else
                throw std::out_of_range("Integral size is larger than expected");
        }
        else
        {
            if constexpr(sizeof(T) <= sizeof(unsigned long))
                result = static_cast<T>(std::strtoul(sv.data(), &endptr, 10));
            else if constexpr(sizeof(T) <= sizeof(unsigned long long))
                result = static_cast<T>(std::strtoull(sv.data(), &endptr, 10));
            else
                throw std::out_of_range("Integral size is larger than expected");
        }

        if(result == 0 && endptr == sv.data())
            throw std::invalid_argument("Unable to parse integral");
        else if(errno == ERANGE)
            throw std::out_of_range("Integral value is out of representable range");
        
        return result;
    }

    template<typename T>
    [[nodiscard]] T to_floating_point(std::string_view sv)
    {
        static_assert(std::is_floating_point_v<T>, "Not a floating-point type");
        T result;
        char* endptr;
        
        if constexpr(std::is_same_v<std::decay_t<T>, float>)
            result = std::strtof(sv.data(), &endptr);
        else if constexpr(std::is_same_v<std::decay_t<T>, double>)
            result = std::strtod(sv.data(), &endptr);
        else
            result = std::strtold(sv.data(), &endptr);

        if(result == 0 && endptr == sv.data())
            throw std::invalid_argument("Unable to parse floating-point type");
        else if(errno == ERANGE)
            throw std::out_of_range("Floating-point value is out of representable range");
        
        return result;
    }
}