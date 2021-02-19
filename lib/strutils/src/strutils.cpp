#include <ien/strutils.hpp>

#include <algorithm>
#include <sstream>
#include <type_traits>

namespace ien::strutils
{
    template<typename TRetVecVal, typename TArg>
    std::vector<TRetVecVal> split(TArg str, char delim)
    {
        static_assert(
            std::is_same_v<std::decay_t<TRetVecVal>, std::string> 
            || std::is_same_v<std::decay_t<TRetVecVal>, std::string_view>
        );
        static_assert(
            std::is_same_v<std::decay_t<TArg>, std::string> 
            || std::is_same_v<std::decay_t<TArg>, std::string_view>
        );

        std::vector<TRetVecVal> result;
        std::decay_t<TArg>::const_iterator it = str.cbegin();
        std::decay_t<TArg>::const_iterator found_it = std::find(it, str.end(), delim);

        while (found_it != str.end())
        {
            if (it != found_it)
            {
                if constexpr (std::is_same_v<std::decay_t<TRetVecVal>, std::string>)
                {
                    result.push_back(std::string(it, found_it));
                }
                else
                {
                    result.push_back(std::string_view(&(*it), std::distance(it, found_it)));
                }
            }
            it = ++found_it;
            found_it = std::find(it, str.end(), delim);
        }
        if (it != str.end())
        {
            if constexpr (std::is_same_v<std::decay_t<TRetVecVal>, std::string>)
            {
                result.push_back(std::string(it, str.end()));
            }
            else
            {
                result.push_back(std::string_view(&(*it), std::distance(it, str.end())));
            }
        }

        return result;
    }

    std::vector<std::string> split(const std::string& str, char delim)
    {
        return split<std::string, const std::string&>(str, delim);        
    }

    std::vector<std::string> split(const std::string_view str, char delim)
    {
        return split<std::string, const std::string_view>(str, delim);
    }

    std::vector<std::string_view> split_view(const std::string& str, char delim)
    {
        return split<std::string_view, const std::string&>(str, delim);
    }

    std::vector<std::string_view> split_view(const std::string_view str, char delim)
    {
        return split<std::string_view, const std::string_view>(str, delim);
    }

    bool contains(const std::string& str, char ocurrence)
    {
        return str.find(ocurrence) != std::string::npos;
    }

    bool contains(const std::string& str, const std::string& ocurrence)
    {
        return str.find(ocurrence) != std::string::npos;
    }

    bool contains(const std::string& str, const std::string_view ocurrence)
    {
        return str.find(ocurrence) != std::string::npos;
    }

    bool contains(const std::string_view str, char ocurrence)
    {
        return str.find(ocurrence) != std::string::npos;
    }

    bool contains(const std::string_view str, const std::string& ocurrence)
    {
        return str.find(ocurrence) != std::string::npos;
    }

    bool contains(const std::string_view str, const std::string_view ocurrence)
    {
        return str.find(ocurrence) != std::string::npos;
    }

    std::string replace(const std::string& str, char ocurrence, char replacement)
    {
        std::string copy = str;
        std::replace(copy.begin(), copy.end(), ocurrence, replacement);
        return copy;
    }

    std::string replace(const std::string_view str, char ocurrence, char replacement)
    {
        std::string copy(str);
        std::replace(copy.begin(), copy.end(), ocurrence, replacement);
        return copy;
    }

    std::string replace(const std::string& str, const std::string& ocurrence, const std::string& replacement)
    {
        std::stringstream sstr;
        
        size_t current_offset = 0;
        size_t idx = str.find(ocurrence, current_offset);
        while(idx != std::string::npos)
        {
            sstr << str.substr(current_offset, (idx - current_offset)) << replacement;
            current_offset = (idx + ocurrence.size());            
            idx = str.find(ocurrence, current_offset);
        }
        if(current_offset == 0) 
        {
            return str;
        }
        else
        {
            sstr << str.substr(current_offset, str.size() - current_offset);
            return sstr.str();
        }
    }

    void replace_in_place(std::string& str, char ocurrence, char replacement)
    {
        std::replace(str.begin(), str.end(), ocurrence, replacement);
    }

    std::string to_upper(std::string_view str)
    {
        std::string result;
        result.reserve(str.size());
        std::transform(str.cbegin(), str.cend(), std::back_inserter(result), std::toupper);
        return result;
    }

    std::string to_lower(std::string_view str)
    {
        std::string result;
        result.reserve(str.size());
        std::transform(str.cbegin(), str.cend(), std::back_inserter(result), std::tolower);
        return result;
    }

    void to_upper_in_place(std::string& str)
    {
        std::transform(str.begin(), str.end(), str.begin(), std::toupper);
    }

    void to_lower_in_place(std::string& str)
    {
        std::transform(str.begin(), str.end(), str.begin(), std::tolower);
    }

    std::string_view trim_start(std::string_view str)
    {
        size_t idx = 0;
        while(str[idx] == ' ') { ++idx; }
        return str.substr(idx);
    }

    std::string_view trim_end(std::string_view str)
    {
        size_t idx = str.size() - 1;
        while(str[idx] == ' ') { --idx; }
        return str.substr(0, idx + 1);
    }

    std::string_view trim(std::string_view str)
    {
        return trim_end(trim_start(str));
    }
}