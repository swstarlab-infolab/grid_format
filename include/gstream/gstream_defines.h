#ifndef _GSTREAM_GSREAM_DEFIMES_H_
#define _GSTREAM_GSREAM_DEFIMES_H_
#include <stdint.h>

namespace gstream {

using gstream_size_t = uint64_t;
using gstream_ssize_t = int64_t;

} // !namespace gstream

#define GSTREAM_DECLARE_HANDLE(name) \
    struct __mixx_handle_name##__ { \
        int __unused;\
    };\
    using name =  __mixx_handle_name##__*

#define GSTREAM_NULL_HANDLE nullptr

#endif //!_GSTREAM_GSREAM_DEFIMES_H_