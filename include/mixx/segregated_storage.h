#ifndef _MIXX_SEGREGATED_STORAGE_H_
#define _MIXX_SEGREGATED_STORAGE_H_
#include <mixx/detail/noncopyable.h>
#include <mixx/stack.h>

namespace mixx {

class segregated_storage final : noncopyable {
public:
    segregated_storage(void* preallocated, mixx_size_t bufsize, mixx_size_t block_size);
    void* allocate();
    void deallocate(void* p) noexcept;
    void reset();

    void* const buffer;
    mixx_size_t const bufsize;
    mixx_size_t const block_size;
    mixx_size_t const capacity;

private:
    stack<void*> _free_list;
};

} // !namespace mixx
#endif // _MIXX_SEGREGATED_STORAGE_H_
