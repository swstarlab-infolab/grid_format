#ifndef _MIXX_BIT_STACK_H_
#define _MIXX_BIT_STACK_H_
#include <mixx/mixx_config.h>
#include <mixx/size.h>

namespace mixx {

class bit_stack {
public:
    explicit bit_stack(mixx_size_t capacity_ = DefaultContainerCapacity);
    bit_stack(bit_stack const& other);
    bit_stack(bit_stack&& other) noexcept;
    ~bit_stack() noexcept;
    bit_stack& operator=(bit_stack const& rhs);
    bit_stack& operator=(bit_stack&& rhs) noexcept;
    bool push(bool value);
    bool pop();
    bool peek() const;
    void clear();
    bool reserve(mixx_size_t new_capacity);

    bool push(unsigned const value) {
        return push(value != 0);
    }

    bool push(int const value) {
        return push(value != 0);
    }

    mixx_size_t size() const {
        return _top;
    }

    bool empty() const {
        return _top == 0;
    }

protected:

    mixx_size_t _cap;
    mixx_size_t _top;
    int* _container;
};

} // !namespace mixx
#endif // !_MIXX_BIT_STACK_H_
