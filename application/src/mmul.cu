#include <mmul.cuh>

void mmul_gpu(cuda_executor<oneway_t, bulk_t, blocking_t::always_t> &ex,
              MatrixXd &a, MatrixXd &b, MatrixXd &c) {
  double *a_d, *b_d, *c_d;

  auto device_upload = [=](void *var, std::size_t size) {
    void *gpu_var;
    cudaMallocManaged(&gpu_var, size);
    memcpy(gpu_var, var, size);
    return gpu_var;
  };

  a_d = static_cast<double *>(
      device_upload(static_cast<void *>(a.data()), a.size() * sizeof(double)));
  b_d = static_cast<double *>(
      device_upload(static_cast<void *>(b.data()), b.size() * sizeof(double)));
  c_d = static_cast<double *>(
      device_upload(static_cast<void *>(c.data()), c.size() * sizeof(double)));

  std::array<int, 6> shape{static_cast<int>(ceil(a.rows() / 2.0)),
                           static_cast<int>(ceil(b.cols() / 2.0)),
                           1,
                           2,
                           2,
                           1};

  auto mul = [=] __device__() {
    unsigned row = blockIdx.y * blockDim.y + threadIdx.y;
    unsigned col = blockIdx.x * blockDim.x + threadIdx.x;
    double sum = 0;
    if (col < b.cols() && row < a.rows()) {
      for (int i = 0; i < a.cols(); i++) {
        sum += a_d[row * a.cols() + i] * b_d[i * b.cols() + col];
      }
      c_d[row * b.cols() + col] = sum;
    }
  };

  ex.bulk_execute(mul, shape);

  memcpy(static_cast<double *>(c.data()), c_d, 9 * sizeof(double));
}
