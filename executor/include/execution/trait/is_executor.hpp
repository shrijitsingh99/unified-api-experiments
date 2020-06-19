//
// Created by Shrijit Singh on 14/06/20.
//

#pragma once

#include <execution/executor/base_executor.hpp>
#include <experimental/type_traits>
#include <type_traits>

namespace execution {

template <class Executor, typename Function = void (*)(), typename = void>
struct is_executor : std::false_type {};

namespace detail {
const auto noop = [] {};
}

template <typename Executor, typename Function>
struct is_executor<
    Executor, Function,
    std::enable_if_t<
        std::is_copy_constructible_v<
            std::remove_cv_t<std::remove_reference_t<Executor>>> &&
            std::is_same_v<
                decltype(std::declval<Executor>().execute(detail::noop)), void>,
        decltype(std::declval<Executor&>() == std::declval<Executor&>(),
                 (void)0)>> : std::true_type {};

template <typename Executor>
constexpr bool is_executor_v = is_executor<Executor>::value;

}  // namespace execution