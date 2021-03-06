/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 *  Point Cloud Library (PCL) - www.pointclouds.org
 *  Copyright (c) 2020-, Open Perception, Inc.
 *  Author: Shrijit Singh <shrijitsingh99@gmail.com>
 *
 */

#pragma once

#include <Eigen/Dense>
#include <array>

#if defined(_OPENMP)
  #include <omp.h>
#endif

#include <executor/executor.h>

#include <mmul.cuh>

using namespace executor;
using namespace Eigen;

template <typename Executor,
          typename executor::InstanceOf<Executor, inline_executor> = 0>
void mmul(const Executor ex, const MatrixXd& a, const MatrixXd& b,
          MatrixXd& c) {
  auto mul = [&]() { c = a * b; };
  ex.execute(mul);
}

template <typename Executor,
          typename executor::InstanceOf<Executor, omp_executor> = 0>
void mmul(const Executor ex, const MatrixXd& a, const MatrixXd& b,
          MatrixXd& c) {
  auto mul = [&](typename decltype(ex)::index_type index) {
#pragma omp for schedule(static)
    for (int i = 0; i < a.rows(); i = i + 1) {
      for (int j = 0; j < b.cols(); j = j + 1) {
        c(i, j) = 0.0;
        for (int k = 0; k < a.cols(); k = k + 1) {
          c(i, j) += a(i, k) * b(k, j);
        }
      }
    }
  };
  ex.bulk_execute(mul, a.rows());
}

template <typename Executor,
          typename executor::InstanceOf<Executor, cuda_executor> = 0>
void mmul(const Executor& ex, const MatrixXd& a, const MatrixXd& b,
          MatrixXd& c) {
  mmul_gpu(ex, a, b, c);
}

void mmul(const MatrixXd& a, const MatrixXd& b, MatrixXd& c,
          bool custom_priority) {
  auto mul = [&](auto& exec) { mmul(exec, a, b, c); };

  if (custom_priority)
    enable_exec_with_priority(mul, inline_executor<>{}, omp_executor<>{});
  else
    enable_exec_on_desc_priority(mul, inline_executor<>{}, omp_executor<>{});
}
