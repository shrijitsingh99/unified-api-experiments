/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 *  Point Cloud Library (PCL) - www.pointclouds.org
 *  Copyright (c) 2014-, Open Perception, Inc.
 *  Author(s): Shrijit Singh <shrijitsingh99@gmail.com>
 *
 */

#pragma once

#include <executor/property/base_property.hpp>

namespace executor {

struct blocking_t {
 public:
  struct always_t : basic_executor_property<always_t, true, true> {
    template <class Executor>
    friend Executor require(Executor& ex, const always_t& t) {
      return ex.require(t);
    }
    template <class Executor>
    friend bool query(const Executor& ex, const always_t& t) {
      return std::is_same<always_t, decltype(ex.interface)>();
    }
  };

  const always_t always;

  struct never_t : basic_executor_property<never_t, true, true> {
    template <class Executor>
    friend Executor require(Executor& ex, const never_t& t) {
      return ex.require(t);
    }
    template <class Executor>
    friend bool query(const Executor& ex, const never_t& t) {
      return std::is_same<never_t, decltype(ex.interface)>();
    }
  };

  const never_t never;

  struct possibly_t : basic_executor_property<possibly_t, true, true> {
    template <class Executor>
    friend Executor require(Executor& ex, const possibly_t& t) {
      return ex.require(t);
    }
    template <class Executor>
    friend bool query(const Executor& ex, const possibly_t& t) {
      return std::is_same<never_t, decltype(ex.interface)>();
    }
  };

  const possibly_t possibly;
};

static constexpr blocking_t blocking{};

}  // namespace executor
