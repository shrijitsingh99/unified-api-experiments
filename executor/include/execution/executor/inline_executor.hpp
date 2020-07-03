//
// Created by Shrijit Singh on 2020-06-14.
//

#pragma once

#include <execution/executor/base_executor.hpp>

template <typename Blocking, typename ProtoAllocator>
struct inline_executor;

namespace execution {
template <>
struct is_executor_available<inline_executor> : std::true_type {};
}  // namespace execution

template <typename Blocking, typename ProtoAllocator = std::allocator<void>>
struct inline_executor : executor<inline_executor, Blocking, ProtoAllocator> {
  using shape_type = std::size_t;

  template <typename F>
  void execute(F &&f) const {
    std::forward<F>(f)();
  }

  inline_executor<blocking_t::always_t, ProtoAllocator> require(
      const blocking_t::always_t &t) const {
    return {};
  }

  static constexpr auto name() { return "inline"; }
};
