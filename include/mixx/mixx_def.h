#ifndef _MIXX_DEF_H_
#define _MIXX_DEF_H_
#include <stdint.h>

namespace mixx {

typedef struct memory_region {
    void* addr;
    uint64_t size;
} memrgn_t;

} // !namespace mixx
#endif // _MIXX_DEF_H_
