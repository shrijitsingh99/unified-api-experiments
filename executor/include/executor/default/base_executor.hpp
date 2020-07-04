/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 *  Point Cloud Library (PCL) - www.pointclouds.org
 *  Copyright (c) 2014-, Open Perception, Inc.
 *  Author(s): Shrijit Singh <shrijitsingh99@gmail.com>
 *
 */

#pragma once

#include <executor/property.h>

#include <array>
#include <executor/trait/is_executor_available.hpp>
#include <executor/trait/is_instance_of_base.hpp>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>

namespace executor {

template <template <typename...> class Derived, typename Blocking,
          typename ProtoAllocator>
struct base_executor {
  template <typename Executor,
            typename executor::instance_of_base<Derived, Executor> = 0>
  bool operator==(const Executor& rhs) const noexcept {
    return std::is_same<Derived<Blocking, ProtoAllocator>, Executor>::value;
  }

  template <typename Executor,
            typename executor::instance_of_base<Derived, Executor> = 0>
  bool operator!=(const Executor& rhs) const noexcept {
    return !operator==(rhs);
  }

  static constexpr bool query(const blocking_t::always_t& t) noexcept {
    return std::is_same<Blocking, blocking_t::always_t>();
  }

  static constexpr bool query(const blocking_t::never_t& t) noexcept {
    return std::is_same<Blocking, blocking_t::never_t>();
  }

  static constexpr bool query(const blocking_t::possibly_t& t) noexcept {
    return std::is_same<Blocking, blocking_t::possibly_t>();
  }

  template <typename F, typename... Args>
  void bulk_execute(F&& f, Args&&... args, std::size_t n) const {
    for (std::size_t i = 0; i < n; ++i) {
      std::forward<F>(f)(std::forward<Args>(args)..., i);
    }
  }
};

}  // namespace executor