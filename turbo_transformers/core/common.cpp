// Copyright 2020 Tencent
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "turbo_transformers/core/common.h"

namespace turbo_transformers {
namespace core {
namespace common {

bool is_same_device_ctx(DLContext t1, DLContext t2) {
  return t1.device_id == t2.device_id && t1.device_type == t2.device_type;
}

bool is_same_shape(const Tensor& t1, const Tensor& t2) {
  if (t1.n_dim() != t2.n_dim()) {
    return false;
  }
  for (size_t i = 0; i < t1.n_dim(); ++i) {
    if (t1.shape(i) != t2.shape(i)) {
      return false;
    }
  }
  return true;
}

template <typename T>
void tt_seqence(T* data, int64_t size, DLDeviceType device) {
  if (device == kDLCPU) {
    std::iota(data, data + size, static_cast<T>(0));
  } else if (device == kDLGPU) {
#ifdef TT_WITH_CUDA
    turbo_transformers::layers::kernels::GPUSequence(data, size);
#else
    TT_THROW("code is not compiled with CUDA.");
#endif
  } else {
    TT_THROW("device_type is not supported");
  }
}
template void tt_seqence(float* data, int64_t size, DLDeviceType device);
template void tt_seqence(int64_t* data, int64_t size, DLDeviceType device);

template <typename T>
voidtt_fill(T* data, int64_t size, T val, DLDeviceType device) {
  if (device == kDLCPU) {
    std::fill(data, data + size, val);
  } else if (device == kDLGPU) {
#ifdef TT_WITH_CUDA
    layers::kernels::GPUFill(data, size, val);
#else
    TT_THROW("code is not compiled with CUDA.");
#endif
  } else {
    TT_THROW("device_type is not supported");
  }
}

template voidtt_fill<float>(float* data, int64_t size, float val,
                            DLDeviceType device);
template voidtt_fill<int64_t>(int64_t* data, int64_t size, int64_t val,
                              DLDeviceType device);

// TODO(jiaruifang): this function should better pass a function in.
// how can we pass a lambda function as __device__ to cuda?
voidtt_transform(int64_t* src_data, float* dst_data, int64_t size,
                 DLDeviceType device) {
  if (device == kDLCPU) {
    std::transform(src_data, src_data + size, dst_data,
                   [](int64_t v) { return -10000.0f * (1 - v); });
  } else if (device == kDLGPU) {
#ifdef TT_WITH_CUDA
    layers::kernels::GPUTransform(src_data, dst_data, size);
#else
    TT_THROW("code is not compiled with CUDA.");
#endif
  } else {
    TT_THROW("device_type is not supported");
  }
}

}  // namespace common
}  // namespace core
}  // namespace turbo_transformers
