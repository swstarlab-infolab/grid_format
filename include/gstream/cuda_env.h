#ifndef _GSTREAM_CUDA_ENV_H_
#define _GSTREAM_CUDA_ENV_H_
#include <mixx/mixx_env.h>

#if defined(__CUDACC__)
#define GSTREAM_DEVICE_COMPATIBLE __host__ __device__
#define GSTREAM_DEVICE_ONLY __device__
#else
#define GSTREAM_DEVICE_COMPATIBLE
#define GSTREAM_DEVICE_ONLY [[deprecated]]
#endif

#if !defined(__CUDACC__) && defined(MIXX_TOOLCHAIN_MSVC)
// http://docs.nvidia.com/cuda/cuda-c-programming-guide/index.html#

template <typename T>
inline T __unused_function_dummy(...) { return T{}; }

#define atomicAdd(address, val) __unused_function_dummy<decltype(val)>(address, val) 
#define __threadfence_block() 1
#define __syncwarp() 1
// https://docs.nvidia.com/cuda/cuda-c-programming-guide/index.html#warp-shuffle-functions
#define __shfl_sync(/*unsigned*/ mask, /*T*/ var, /*int*/ srcLane, /*int*/ width /*= warpSize*/) 1
#define GSTREAM_CUDA_KERNEL_CALL_2(Kernel, GridDim, BlockDim) \
    _mixx_unused(GridDim); _mixx_unused(BlockDim); Kernel
#define GSTREAM_CUDA_KERNEL_CALL_3(Kernel, GridDim, BlockDim, Shm) \
    _mixx_unused(GridDim); _mixx_unused(BlockDim); _mixx_unused(Shm); Kernel
#define GSTREAM_CUDA_KERNEL_CALL_4(Kernel, GridDim, BlockDim, Shm, Stream) \
    _mixx_unused(GridDim); _mixx_unused(BlockDim); _mixx_unused(Shm); _mixx_unused(Stream); Kernel
#else
#define GSTREAM_CUDA_KERNEL_CALL_2(Kernel, GridDim, BlockDim) Kernel<<<GridDim, BlockDim>>>
#define GSTREAM_CUDA_KERNEL_CALL_3(Kernel, GridDim, BlockDim, Shm) Kernel<<<GridDim, BlockDim, Shm>>>
#define GSTREAM_CUDA_KERNEL_CALL_4(Kernel, GridDim, BlockDim, Shm, Stream) Kernel<<<GridDim, BlockDim, Shm, Stream>>>
#endif // !__INTELLISENSE__

#define __GSTREAM_CUDA_KERNEL_CALL_N(N) GSTREAM_CUDA_KERNEL_CALL_##N
#define _GSTREAM_CUDA_KERNEL_CALL_N(N) __GSTREAM_CUDA_KERNEL_CALL_N(N)
#define GSTREAM_CUDA_KERNEL_CALL(Kernel, ...) \
    MIXX_PP_EXPAND(_GSTREAM_CUDA_KERNEL_CALL_N(MIXX_PP_NARG(__VA_ARGS__)) (Kernel, __VA_ARGS__))

#endif // !_GSTREAM_CUDA_ENV_H_
