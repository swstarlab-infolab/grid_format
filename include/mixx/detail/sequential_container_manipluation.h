#ifndef _MIXX_DETAIL_ARRAY_COPY_H_
#define _MIXX_DETAIL_ARRAY_COPY_H_
#include <mixx/mixx_env.h>
#include <string.h>
#include <type_traits>
#include <utility>

namespace mixx {

namespace detail {

namespace _seq_cont_manipulation {

template <typename T>
using pod_type_tracing = std::integral_constant<bool, std::is_pod<T>::value>;

template <typename T>
using pod_type = std::integral_constant<bool, true>;

template <typename T>
using non_pod_t = std::integral_constant<bool, false>;

template <typename T>
void copy_array(T* __restrict dst, T* __restrict src, mixx_size_t size, pod_type<T>) {
    memcpy(dst, src, sizeof(T) * size);
}

template <typename T>
void copy_array(T* __restrict dst, T* __restrict src, mixx_size_t size, non_pod_t<T>) {
    for (mixx_size_t i = 0; i < size; ++i)
        dst[i] = src[i];
}

template <typename T>
void move_array(T* __restrict dst, T* __restrict src, mixx_size_t size, pod_type<T>) {
    memcpy(dst, src, sizeof(T) * size);
}

template <typename T>
void move_array(T* __restrict dst, T* __restrict src, mixx_size_t size, non_pod_t<T>) {
    for (mixx_size_t i = 0; i < size; ++i)
        dst[i] = std::move(src[i]);
}

template <typename T>
void clear_array(T* dst, mixx_size_t size, pod_type<T>) {
    memset(dst, 0, sizeof(T) * size);
}

template <typename T>
void clear_array(T* dst, mixx_size_t size, non_pod_t<T>) {
    for (mixx_size_t i = 0; i < size; ++i)
        dst.~T();
}

} // !namespace _seq_cont_manipulation

} // !namespace detail

template <typename T>
void copy_array(T* __restrict dst, T* __restrict src, mixx_size_t size) {
    using namespace detail;
    using namespace _seq_cont_manipulation;
    copy_array(dst, src, size, pod_type_tracing<T>());
}

template <typename T>
void move_array(T* __restrict dst, T* __restrict src, mixx_size_t size) {
    using namespace detail;
    using namespace _seq_cont_manipulation;
    copy_array(dst, src, size, pod_type_tracing<T>());
}

template <typename T>
void clear_array(T* dst, mixx_size_t size) {
    using namespace detail;
    using namespace _seq_cont_manipulation;
    clear_array(dst, size, pod_type_tracing<T>());
}

} // !namespace mixx

#endif // _MIXX_DETAIL_ARRAY_COPY_H_
