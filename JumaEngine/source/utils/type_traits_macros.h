// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include <type_traits>

#define TEMPLATE_ENABLE(...) std::enable_if_t<__VA_ARGS__>* = nullptr

template<typename T1, typename T2>
constexpr bool is_not_same = !std::is_same_v<std::remove_cv_t<T1>, std::remove_cv_t<T2>>;

template<typename Base, typename Child>
constexpr bool is_base_and_not_same = std::is_base_of_v<Base, Child> && is_not_same<Base, Child>;

template<typename Base, typename Child>
constexpr bool is_base_and_not_abstract = std::is_base_of_v<Base, Child> && !std::is_abstract_v<Child>;
