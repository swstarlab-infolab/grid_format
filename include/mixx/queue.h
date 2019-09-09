#ifndef _MIXX_QUEUE_H_
#define _MIXX_QUEUE_H_
#include <mixx/detail/sequential_container_manipluation.h>
#include <mixx/size.h>
#include <mixx/mixx_config.h>
#include <assert.h>
#include <memory>

namespace mixx {

template <typename ValueType, typename Allocator = std::allocator<ValueType>>
class queue {
public:
    using value_type = ValueType;
    using alloc_type = Allocator;
    explicit queue(mixx_size_t capacity_ = DefaultContainerCapacity, alloc_type const& alloc = alloc_type());
    queue(queue const& other);
    queue(queue&& other) noexcept;
    ~queue() noexcept;
    queue& operator=(queue const& rhs);
    queue& operator=(queue&& rhs) noexcept;
    mixx_size_t push(value_type const& lvalue);
    mixx_size_t push(value_type&& rvalue);
    template <typename ...Args> mixx_size_t emplace(Args&& ...);
    bool pop();
    bool pop(value_type*);
    MIXX_FORCEINLINE value_type& peek();
    MIXX_FORCEINLINE bool empty() const;
    MIXX_FORCEINLINE mixx_size_t capacity() const;
    MIXX_FORCEINLINE mixx_size_t size() const;
    void clear();
    void reserve(mixx_size_t new_capacity);

    value_type* _get_buffer();

protected:
    void _release_container();
    value_type* _acquire_buffer();
    MIXX_FORCEINLINE void _move_front();
    MIXX_FORCEINLINE void _move_rear();

    alloc_type _alloc;
    mixx_size_t _cap;
    mixx_size_t _front;
    mixx_size_t _rear;
    mixx_size_t _size;
    value_type* _cont;
};

template <typename ValueType, typename Allocator>
queue<ValueType, Allocator>::queue(mixx_size_t const capacity_, alloc_type const& alloc):
    _alloc(alloc) {
    _cap = roundup2(capacity_);
    _front = 0;
    _rear = 0;
    _size = 0;
    _cont = std::allocator_traits<alloc_type>::allocate(_alloc, _cap);
}

template <typename ValueType, typename Allocator>
queue<ValueType, Allocator>::queue(queue const& other):
    _alloc(other._alloc) {
    _cap = other._cap; _front = other._front; _rear = other._rear; _size = other._size;
    _cont = std::allocator_traits<alloc_type>::allocate(_alloc, _cap);
    copy_array(_cont, other._cont, _size);
}

template <typename ValueType, typename Allocator>
queue<ValueType, Allocator>::queue(queue&& other) noexcept:
    _alloc(std::move(other._alloc)) {
    _cap = other._cap; _front = other._front; _rear = other._rear; _size = other._size;
    _cont = other._cont;
    other._cont = nullptr;
}

template <typename ValueType, typename Allocator>
queue<ValueType, Allocator>::~queue() noexcept {
    _release_container();
}

template <typename ValueType, typename Allocator>
queue<ValueType, Allocator>& queue<ValueType, Allocator>::operator=(queue const& rhs) {
    _release_container();
    _alloc = rhs._alloc;
    _cap = rhs._cap; _front = rhs._front; _rear = rhs._rear; _size = rhs._size;
    _cont = std::allocator_traits<alloc_type>::allocate(_alloc, _cap);
    copy_array(_cont, rhs._cont, _size);
    return *this;
}

template <typename ValueType, typename Allocator>
queue<ValueType, Allocator>& queue<ValueType, Allocator>::operator=(queue&& rhs) noexcept {
    _release_container();
    _alloc = std::move(rhs._alloc);
    _cap = rhs._cap; _front = rhs._front; _rear = rhs._rear; _size = rhs._size;
    _cont = rhs._cont;
    rhs._cont = nullptr;
    return *this;
}

template <typename ValueType, typename Allocator>
mixx_size_t queue<ValueType, Allocator>::push(value_type const& lvalue) {
    value_type* buf = _acquire_buffer();
    new (buf) value_type(lvalue);
    return ++_size;
}

template <typename ValueType, typename Allocator>
mixx_size_t queue<ValueType, Allocator>::push(value_type&& rvalue) {
    value_type* buf = _acquire_buffer();
    new (buf) value_type(std::move(rvalue));
    return ++_size;
}

template <typename ValueType, typename Allocator> 
template <typename ... Args>
mixx_size_t queue<ValueType, Allocator>::emplace(Args&&... args) {
    value_type* buf = _acquire_buffer();
    new (buf) value_type(std::forward<Args>(args)...);
    return ++_size;
}

template <typename ValueType, typename Allocator>
bool queue<ValueType, Allocator>::pop() {
    if (empty()) return false;
    value_type& val = peek();
    val.~value_type();
    _move_front();
    _size -= 1;
    return true;
}

template <typename ValueType, typename Allocator>
bool queue<ValueType, Allocator>::pop(value_type* out) {
    if (empty()) return false;
    value_type& val = peek();
    *out = std::move(val);
    val.~value_type();
    _move_front();
    _size -= 1;
    return true;
}

template <typename ValueType, typename Allocator>
typename queue<ValueType, Allocator>::value_type& queue<ValueType, Allocator>::peek() {
    return _cont[_front];
}

template <typename ValueType, typename Allocator>
bool queue<ValueType, Allocator>::empty() const {
    return _size == 0;
}

template <typename ValueType, typename Allocator>
mixx_size_t queue<ValueType, Allocator>::capacity() const {
    return _cap;
}

template <typename ValueType, typename Allocator>
mixx_size_t queue<ValueType, Allocator>::size() const {
    return _size;
}

template <typename ValueType, typename Allocator>
void queue<ValueType, Allocator>::clear() {
    mixx_size_t s = _cap - _rear;
    clear_array(&_cont[_rear], s);
    s = _size - s;
    if (s > 0) 
        clear_array(_cont, s);
    _size = 0;
    _front = 0;
    _rear = 0;
}

template <typename ValueType, typename Allocator>
void queue<ValueType, Allocator>::reserve(mixx_size_t new_capacity) {
    if (_cap >= new_capacity) return;
    new_capacity = roundup2(new_capacity);
    value_type* old = _cont;
    _cont = std::allocator_traits<alloc_type>::allocate(_alloc, new_capacity);

    mixx_size_t s1 = _cap - _rear;
    copy_array(_cont, &old[_rear], s1);
    mixx_size_t s2 = _size - s1;
    if (s2 > 0)
        copy_array(_cont + s1, old, s2);
    _front = _size;
    _rear = 0;
    
    std::allocator_traits<alloc_type>::deallocate(_alloc, old, _cap);
    _cap = new_capacity;
}

template <typename ValueType, typename Allocator>
typename queue<ValueType, Allocator>::value_type* queue<ValueType, Allocator>::_get_buffer() {
    return _cont;
}

template <typename ValueType, typename Allocator>
void queue<ValueType, Allocator>::_release_container() {
    if (_cont != nullptr)
        std::allocator_traits<alloc_type>::deallocate(_alloc, _cont, _cap);
}

template <typename ValueType, typename Allocator>
typename queue<ValueType, Allocator>::value_type* queue<ValueType, Allocator>::_acquire_buffer() {
    if (_size == _cap) {
        reserve(_cap * 2);
    }
    value_type* v = &_cont[_front];
    _move_front();
    return v;
}

template <typename ValueType, typename Allocator>
void queue<ValueType, Allocator>::_move_front() {
    _front = (_front + 1) % _cap;
}

template <typename ValueType, typename Allocator>
void queue<ValueType, Allocator>::_move_rear() {
    _rear = (_rear + 1) % _cap;
}

} // !namespace mixx

#endif // _MIXX_QUEUE_H_
