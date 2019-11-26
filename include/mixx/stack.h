#ifndef _MIXX_STACK_H_
#define _MIXX_STACK_H_
#include <mixx/detail/sequential_container_manipluation.h>
#include <mixx/size.h>
#include <mixx/mixx_config.h>
#include <assert.h>
#include <memory>

namespace mixx {

template <typename ValueType, typename Allocator = std::allocator<ValueType>>
class stack {
public:
    using value_type = ValueType;
    using alloc_type = Allocator;
    explicit stack(mixx_size_t capacity_ = DefaultContainerCapacity, alloc_type const& alloc = alloc_type());
    stack(stack const& other);
    stack(stack&& other) noexcept;
    ~stack() noexcept;
    stack& operator=(stack const& rhs);
    stack& operator=(stack&& rhs) noexcept;
    mixx_size_t push(value_type const& lvalue);
    mixx_size_t push(value_type&& rvalue);
    template <typename ...Args> mixx_size_t emplace(Args&& ...);
    bool pop();
    bool pop(value_type*);
    value_type& peek();
    bool empty() const;
    mixx_size_t capacity() const;
    mixx_size_t size() const;
    void clear();
    void reserve(mixx_size_t new_capacity);
    void iterate(void(*f)(value_type&));

    void _config(mixx_size_t top);
    value_type* _get_buffer();

protected:
    void _release_container();
    value_type* _acquire_buffer();

    alloc_type _alloc;
    mixx_size_t _cap;
    mixx_size_t _top;
    value_type* _cont;
};

template <typename ValueType, typename Allocator>
stack<ValueType, Allocator>::stack(mixx_size_t const capacity_, alloc_type const& alloc) :
    _alloc(alloc) {
    _cap = roundup2(capacity_);
    _top = 0;
    _cont = std::allocator_traits<alloc_type>::allocate(_alloc, _cap);
}

template <typename ValueType, typename Allocator>
stack<ValueType, Allocator>::stack(stack const& other) :
    _alloc(other._alloc) {
    _cap = other._cap;
    _top = other._top;
    _cont = std::allocator_traits<alloc_type>::allocate(_alloc, _cap);
    copy_array(_cont, other._cont, _top);
}

template <typename ValueType, typename Allocator>
stack<ValueType, Allocator>::stack(stack&& other) noexcept :
    _alloc(std::move(other._alloc)) {
    _cap = other._cap;
    _top = other._top;
    _cont = other._cont;
    other._cont = nullptr;
}

template <typename ValueType, typename Allocator>
stack<ValueType, Allocator>::~stack() noexcept {
    _release_container();
}

template <typename ValueType, typename Allocator>
stack<ValueType, Allocator>& stack<ValueType, Allocator>::operator=(stack const& rhs) {
    _release_container();
    _alloc = rhs._alloc;
    _cap = rhs._cap;
    _top = rhs._top;
    _cont = std::allocator_traits<alloc_type>::allocate(_alloc, _cap);
    copy_array(_cont, rhs._cont, _top);
    return *this;
}

template <typename ValueType, typename Allocator>
stack<ValueType, Allocator>& stack<ValueType, Allocator>::operator=(stack&& rhs) noexcept {
    _release_container();
    _alloc = std::move(rhs._alloc);
    _cap = rhs._cap;
    _top = rhs._top;
    _cont = rhs._cont;
    rhs._cont = nullptr;
    return *this;
}

template <typename ValueType, typename Allocator>
mixx_size_t stack<ValueType, Allocator>::push(value_type const& lvalue) {
    value_type* buf = _acquire_buffer();
    new (buf) value_type(lvalue);
    return _top - 1;
}

template <typename ValueType, typename Allocator>
mixx_size_t stack<ValueType, Allocator>::push(value_type&& rvalue) {
    value_type* buf = _acquire_buffer();
    new (buf) value_type(std::move(rvalue));
    return _top - 1;
}

template <typename ValueType, typename Allocator>
template <typename ... Args>
mixx_size_t stack<ValueType, Allocator>::emplace(Args&&... args) {
    value_type* buf = _acquire_buffer();
    new (buf) value_type(std::forward<Args>(args)...);
    return _top - 1;
}

template <typename ValueType, typename Allocator>
bool stack<ValueType, Allocator>::pop() {
    if (empty()) return false;
    value_type& val = peek();
    val.~value_type();
    _top -= 1;
    return true;
}

template <typename ValueType, typename Allocator>
bool stack<ValueType, Allocator>::pop(value_type* out) {
    if (empty()) return false;
    value_type& val = peek();
    *out = std::move(val);
    val.~value_type();
    _top -= 1;
    return true;
}

template <typename ValueType, typename Allocator>
typename stack<ValueType, Allocator>::value_type& stack<ValueType, Allocator>::peek() {
    assert(_top > 0);
    return _cont[_top - 1];
}

template <typename ValueType, typename Allocator>
bool stack<ValueType, Allocator>::empty() const {
    return _top == 0;
}

template <typename ValueType, typename Allocator>
mixx_size_t stack<ValueType, Allocator>::capacity() const {
    return _cap;
}

template <typename ValueType, typename Allocator>
mixx_size_t stack<ValueType, Allocator>::size() const {
    return _top;
}

template <typename ValueType, typename Allocator>
void stack<ValueType, Allocator>::clear() {
    clear_array(_cont, size());
}

template <typename ValueType, typename Allocator>
void stack<ValueType, Allocator>::reserve(mixx_size_t new_capacity) {
    if (_cap >= new_capacity) return;
    new_capacity = roundup2(new_capacity);
    assert(_top < new_capacity);
    value_type* old = _cont;
    _cont = std::allocator_traits<alloc_type>::allocate(_alloc, new_capacity);
    copy_array(_cont, old, size());
    std::allocator_traits<alloc_type>::deallocate(_alloc, old, _cap);
    _cap = new_capacity;
}

template <typename ValueType, typename Allocator>
void stack<ValueType, Allocator>::iterate(void(*f)(value_type&)) {
    if (size() == 0)
        return;
    for (size_t i = _top; i != 0; --i) {
        f(_cont[i - 1]);
    }
}

template <typename ValueType, typename Allocator>
void stack<ValueType, Allocator>::_config(mixx_size_t const top) {
    _top = top;
}

template <typename ValueType, typename Allocator>
typename stack<ValueType, Allocator>::value_type* stack<ValueType, Allocator>::_get_buffer() {
    return _cont;
}

template <typename ValueType, typename Allocator>
void stack<ValueType, Allocator>::_release_container() {
    if (_cont != nullptr)
        std::allocator_traits<alloc_type>::deallocate(_alloc, _cont, _cap);
}

template <typename ValueType, typename Allocator>
typename stack<ValueType, Allocator>::value_type* stack<ValueType, Allocator>::_acquire_buffer() {
    if (_top == _cap)
        reserve(_cap * 2);
    return &_cont[_top++];
}

} // !namespace mixx

#endif // !_MIXX_STACK_H_
