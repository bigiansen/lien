#pragma once

#include <type_traits>
#include <string>

namespace ien::tt
{
    template<typename T>
    using decay = std::decay_t<T>;

    template<typename TFirst, typename... TRest>
    constexpr bool is_same_decay = (std::is_same_v<decay<TFirst>, decay<TRest>> && ...);

    template<typename T1, typename T2>
    using enable_if_is_type = std::enable_if_t<is_same_decay<T1, T2>>;

    template<typename, typename = void>
    struct is_iterable : std::false_type { };

    template<typename T>
    struct is_iterable<
        T, 
        std::void_t<
            decltype(std::declval<T>().begin()),
            decltype(std::declval<T>().end())
        >
    > : std::true_type { };

    template<typename T, typename = tt::enable_if_is_iterable<T>>
    using iterable_value_type = decltype(*(std::declval<T>().begin()));

    template<typename T, typename = tt::enable_if_is_iterable<T>>
    using iterable_iterator_type = decltype((std::declval<T>().begin()));

    template<typename T, typename TElem>
    constexpr bool is_iterable_of = 
        is_iterable<T>::value && is_same_decay<iterable_value_type<T>, TElem>;

    template<typename T>
    using enable_if_is_iterable = std::enable_if_t<is_iterable<T>::value>;

    template<typename T, typename TElem>
    using enable_if_is_iterable_of = std::enable_if_t<
        is_iterable<T>::value 
        && is_same_decay<iterable_value_type<T>, TElem>>;
}