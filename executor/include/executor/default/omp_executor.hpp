/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 *  Point Cloud Library (PCL) - www.pointclouds.org
 *  Copyright (c) 2020-, Open Perception, Inc.
 *  Author: Shrijit Singh <shrijitsingh99@gmail.com>
 *
 */

#pragma once

#ifdef _OPENMP
  #include <omp.h>
#endif

#include <executor/property.h>
#include <executor/type_trait.h>

namespace executor {

template <typename Blocking, typename ProtoAllocator>
struct omp_executor;

#ifdef _OPENMP
template <>
struct is_executor_available<omp_executor> : std::true_type {};
#endif

template <typename Blocking = blocking_t::always_t,
          typename ProtoAllocator = std::allocator<void>>
struct omp_executor {
  using shape_type = std::size_t;

  using index_type = struct {
    shape_type max;
    int idx;
  };

  std::size_t max_threads = 0;

  omp_executor(): omp_executor(0) {}

  omp_executor(std::size_t max_threads): max_threads(max_threads) {
#ifdef _OPENMP
    set_max_threads(max_threads);
#endif
  }

  bool set_max_threads(std::size_t max_threads) {
#ifdef _OPENMP
    if (!max_threads) this->max_threads = omp_get_max_threads();
    else this->max_threads = max_threads;
    return true;
#endif
    return false;
  }

  template <typename Executor, InstanceOf<Executor, omp_executor> = 0>
  friend bool operator==(const omp_executor& lhs,
                         const Executor& rhs) noexcept {
    return std::is_same<omp_executor, Executor>::value;
  }

  template <typename Executor, InstanceOf<Executor, omp_executor> = 0>
  friend bool operator!=(const omp_executor& lhs,
                         const Executor& rhs) noexcept {
    return !operator==(lhs, rhs);
  }

  template <typename F>
  void execute(F&& f) const {
    static_assert(is_executor_available_v<omp_executor>, "OpenMP executor unavailable");
    f();
  }

  template <typename F>
  void bulk_execute(F&& f, const shape_type& n) const {
    static_assert(is_executor_available_v<omp_executor>, "OpenMP executor unavailable");
#ifdef _OPENMP
    const auto num_threads = n ? std::min(max_threads, n): max_threads;
  #pragma omp parallel num_threads(num_threads)
    {
      index_type index{num_threads, omp_get_thread_num()};
      f(index);
    }
#endif
  }

  static constexpr auto query(const blocking_t&) noexcept { return Blocking{}; }

  omp_executor<blocking_t::always_t, ProtoAllocator> require(
      const blocking_t::always_t&) const {
    return {};
  }

  static constexpr auto name() { return "omp_executor"; }
};

using default_omp_executor = omp_executor<>;

}  // namespace executor
