// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include <type_traits>

#define TEMPLATE_ENABLE_IMPL(...) std::enable_if_t<__VA_ARGS__>*
#define TEMPLATE_ENABLE(...) TEMPLATE_ENABLE_IMPL(__VA_ARGS__) = nullptr

namespace jutils
{
    template<typename Base, typename Child>
    constexpr bool is_base = std::is_base_of_v<Base, Child>;
    template<typename T1, typename T2>
    constexpr bool is_same = std::is_same_v<std::remove_cv_t<T1>, std::remove_cv_t<T2>>;

    template<typename T1, typename T2>
    constexpr bool is_not_same = !is_same<T1, T2>;

    template<typename Base, typename Child>
    constexpr bool is_base_and_not_same = is_base<Base, Child> && is_not_same<Base, Child>;

    template<typename Base, typename Child>
    constexpr bool is_base_and_not_abstract = is_base<Base, Child> && !std::is_abstract_v<Child>;

    template<typename Base, typename Child>
    constexpr bool is_base_and_not_same_and_not_abstract = is_base_and_not_same<Base, Child> && !std::is_abstract_v<Child>;

    template<typename From, typename To>
    constexpr bool is_castable = std::is_convertible_v<From, To>;
}
