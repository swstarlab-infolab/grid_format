#ifndef _GSTREAM_CUDA_ENV_H_
#define _GSTREAM_CUDA_ENV_H_

#if defined(__CUDACC__)
#define GSTREAM_DEVICE_COMPATIBLE __host__ __device__
#define GSTREAM_DEVICE_ONLY __device__
#else
#define GSTREAM_DEVICE_COMPATIBLE
#define GSTREAM_DEVICE_ONLY [[deprecated]]
#endif

#endif // !_GSTREAM_CUDA_ENV_H_
