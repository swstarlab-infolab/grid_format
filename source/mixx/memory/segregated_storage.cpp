#include <mixx/segregated_storage.h>
#include <mixx/mixx_env.h>
#include <assert.h>

namespace mixx {

segregated_storage::segregated_storage(
    void* preallocated, mixx_size_t _bufsize, mixx_size_t _block_size) :
    buffer(preallocated), bufsize(_bufsize), block_size(_block_size),
    capacity(bufsize / block_size), _free_list(capacity) {
    reset();
}

void* segregated_storage::allocate() {
    void* p;
    if (_free_list.pop(&p)) {
        assert(static_cast<char*>(p) >= static_cast<char*>(buffer) &&
            static_cast<char*>(p) < static_cast<char*>(buffer) + bufsize);
        return p;
    }
    return nullptr;
}

void segregated_storage::deallocate(void* p) {
    assert(static_cast<char*>(p) >= static_cast<char*>(buffer) &&
        static_cast<char*>(p) < static_cast<char*>(buffer) + bufsize);
    _free_list.push(p);
}

void segregated_storage::reset() {
    void** p = _free_list._get_buffer();
    for (mixx_size_t i = 0; i < capacity; ++i)
        p[i] = seek_pointer(buffer, block_size * i);
    _free_list._config(capacity);
}

double segregated_storage::fill_rate() const {
    return static_cast<double>(_free_list.size()) / static_cast<double>(_free_list.capacity());
}

} // !namespace mixx