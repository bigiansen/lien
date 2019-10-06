#include <ien/strutils.hpp>

#include <algorithm>
#include <sstream>

namespace ien::strutils
{
    std::vector<std::string> split(const std::string& str, char delim)
    {
        std::vector<std::string> result;

        std::string::const_iterator it = str.cbegin();
        for(;;)
        {
            std::string::const_iterator found_it = std::find(it, str.end(), delim);
            if(found_it == str.end()) 
            { 
                break;
            }
            else
            {
                if(it != found_it)
                {
                    result.push_back(std::string(it, found_it));
                }
                it = ++found_it;
            }
        }
        if(it != str.end())
        {
            result.push_back(std::string(it, str.end()));
        }

        return result;
    }

    std::vector<std::string> split(const std::string_view str, char delim)
    {
        std::vector<std::string> result;

        std::string_view::const_iterator it = str.cbegin();
        for(;;)
        {
            std::string_view::const_iterator found_it = std::find(it, str.end(), delim);
            if(found_it == str.end()) 
            { 
                break;
            }
            else
            {
                if(it != found_it)
                {
                    result.push_back(std::string(it, found_it));
                }
                it = ++found_it;
            }
        }
        if(it != str.end())
        {
            result.push_back(std::string(it, str.end()));
        }

        return result;
    }

    std::vector<std::string_view> split_view(const std::string& str, char delim)
    {
        std::vector<std::string_view> result;

        std::string::const_iterator it = str.cbegin();
        for(;;)
        {
            std::string::const_iterator found_it = std::find(it, str.end(), delim);
            if(found_it == str.end()) 
            { 
                break;
            }
            else
            {
                if(it != found_it)
                {
                    result.push_back(std::string_view(&(*it), std::distance(it, found_it)));
                }
                it = ++found_it;
            }
        }
        if(it != str.end())
        {
            result.push_back(std::string_view(&(*it), std::distance(it, str.end())));
        }

        return result;
    }

    std::vector<std::string_view> split_view(const std::string_view str, char delim)
    {
        std::vector<std::string_view> result;

        std::string_view::const_iterator it = str.cbegin();
        for(;;)
        {
            std::string_view::const_iterator found_it = std::find(it, str.end(), delim);
            if(found_it == str.end()) 
            { 
                break;
            }
            else
            {
                if(it != found_it)
                {
                    result.push_back(std::string_view(&(*it), std::distance(it, found_it)));
                }
                it = ++found_it;
            }
        }
        if(it != str.end())
        {
            result.push_back(std::string_view(&(*it), std::distance(it, str.end())));
        }

        return result;
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
        for(;;)
        {
            size_t idx = str.find(ocurrence, current_offset);
            if(idx == std::string::npos)
            {
                break;
            }
            else
            {
                sstr << str.substr(current_offset, (idx - current_offset)) << replacement;
                current_offset = (idx + ocurrence.size());
            }
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
        std::transform(str.cbegin(), str.cend(), std::back_inserter(result), toupper);
        return result;
    }

    std::string to_lower(std::string_view str)
    {
        std::string result;
        result.reserve(str.size());
        std::transform(str.cbegin(), str.cend(), std::back_inserter(result), tolower);
        return result;
    }

    void to_upper_in_place(std::string& str)
    {
        std::transform(str.begin(), str.end(), str.begin(), toupper);
    }

    void to_lower_in_place(std::string& str)
    {
        std::transform(str.begin(), str.end(), str.begin(), tolower);
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