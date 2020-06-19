//
// Created by Shrijit Singh on 2020-06-14.
//

#pragma once

#include <execution/executor/base_executor.hpp>

template <typename Interface, typename Cardinality, typename Blocking,
          typename ProtoAllocator>
struct sse_executor;

#ifdef __SSE__
template <>
struct execution::is_executor_available<sse_executor> : std::true_type {};
#endif

template <typename Interface, typename Cardinality, typename Blocking,
          typename ProtoAllocator = std::allocator<void>>
struct sse_executor
    : executor<sse_executor, Interface, Cardinality, Blocking, ProtoAllocator> {
  using shape_type = std::size_t;

  template <typename F>
  void execute(F &&f) {
    std::invoke(std::forward<F>(f));
  }

  template <typename F>
  void bulk_execute(F &&f, shape_type n) {
#pragma simd
    for (std::size_t i = 0; i < n; ++i) {
      std::invoke(std::forward<F>(f), i);
    }
  }

  auto decay_t() -> decltype(auto) {
    if constexpr (execution::is_executor_available_v<sse_executor>) {
      return *this;
    } else
      return inline_executor<oneway_t, blocking_t::always_t, ProtoAllocator>{};
  }

  std::string name() { return "sse"; }
};
