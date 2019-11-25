#ifndef _MIXX_DETAIL_DATA_INITIALIZATION_H_
#define _MIXX_DETAIL_DATA_INITIALIZATION_H_
#include <mixx/mixx_env.h>
#include <utility>

namespace mixx {

template <typename T, typename ...Args>
MIXX_FORCEINLINE void placement_new(T* p, Args&& ...args) {
    new (p) T(std::forward<Args>(args)...);
}

template <typename T>
MIXX_FORCEINLINE void placement_delete(T* p) {
    p->~T();
}

} // !namespace mixx
#endif // _MIXX_DETAIL_DATA_INITIALIZATION_H_
