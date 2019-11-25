#include <mixx/bit_stack.h>
#include <mixx/bitset.h>
#include <string.h>
#include <assert.h>

namespace mixx {

bit_stack::bit_stack(mixx_size_t capacity_) {
    _cap = roundup(capacity_, 4llu);
    _top = 0;
    _container = static_cast<int*>(malloc(_cap / sizeof(int)));
    if (_container != nullptr)
        memset(_container, 0, _cap / sizeof(int));
}

bit_stack::bit_stack(bit_stack const& other) {
    _cap = other._cap;
    _top = other._top;
    _container = static_cast<int*>(malloc(_cap / sizeof(int)));
    if (_container != nullptr)
        memcpy(_container, other._container, _cap / sizeof(int));
}

bit_stack::bit_stack(bit_stack&& other) noexcept {
    _cap = other._cap;
    _top = other._top;
    _container = other._container;
    other._container = nullptr;
}


bit_stack::~bit_stack() noexcept {
    free(_container);
}

bit_stack& bit_stack::operator=(bit_stack const& rhs) {
    _cap = rhs._cap;
    _top = rhs._top;
    _container = static_cast<int*>(malloc(_cap / sizeof(int)));
    if (_container != nullptr)
        memcpy(_container, rhs._container, _cap / sizeof(int));
    return *this;
}

bit_stack& bit_stack::operator=(bit_stack&& rhs) noexcept {
    _cap = rhs._cap;
    _top = rhs._top;
    _container = rhs._container;
    rhs._container = nullptr;
    return *this;
}

bool bit_stack::push(bool const value) {
    if (_top == _cap) {
        if (!reserve(_cap * 2))
            return false;
        _top += 1;
    }
    if (value)
        set_bit(_container, _top);
    else
        clear_bit(_container, _top);
    _top += 1;
    return true;
}

bool bit_stack::pop() {
    if (empty())
        return false;
    _top -= 1;
    return true;
}

bool bit_stack::peek() const {
    assert(_top > 0);
    return test_bit(_container, _top - 1);
}

void bit_stack::clear() {
    _top = 0;
    if (_container != nullptr)
        memset(_container, 0, _cap / sizeof(int));
}

bool bit_stack::reserve(mixx_size_t const new_capacity) {
    if (_cap >= new_capacity)
        return true;
    assert(_cap % 4 == 0);
    assert(_container != nullptr);
    int* new_container = static_cast<int*>(malloc(new_capacity / sizeof(int)));
    if (new_container == nullptr)
        return false;
    memcpy(new_container, _container, size() / sizeof(int));
    free(_container);
    _container = new_container;
    return true;
}

} // !namespace mixx