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
    [[nodiscard]] T string_view_to_integral(std::string_view sv)
    {
        static_assert(std::is_integral_v<T>, "Not an integral type");
        T result;
        std::from_chars_result op_result = std::from_chars(sv.data(), sv.data() + sv.size(), result);
        if(op_result.ec == std::errc::invalid_argument)
        {
            throw std::invalid_argument("String view does not represent an integral type");
        }
        return result;
    }

    template<typename T>
    [[nodiscard]] T string_view_to_float(std::string_view sv)
    {
        static_assert(std::is_floating_point_v<T>, "Not a floating-point type");
        T result;
        std::from_chars_result op_result = std::from_chars(sv.data(), sv.data() + sv.size(), result);
        if(op_result.ec == std::errc::invalid_argument)
        {
            throw std::invalid_argument("String view does not represent a float type");
        }
        return result;
    }
}