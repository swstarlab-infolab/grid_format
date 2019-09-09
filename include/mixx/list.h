#ifndef _MIXX_H_LIST_H_
#define _MIXX_H_LIST_H_
#include <mixx/detail/noncopyable.h>
#include <mixx/mixx_env.h>
#include <memory>

namespace mixx {

template <typename T>
struct __list_node {
    using value_type = T;
    value_type value;
    __list_node* prev;
    __list_node* next;
};

template <typename T>
class list_iterator {
public:
    using node_type = __list_node<T>;
    using value_type = T;
    explicit list_iterator(void* p = nullptr) {
        _p = static_cast<node_type*>(p);
    }

    MIXX_FORCEINLINE list_iterator& operator++() {
        _p = _p->next;
        return *this;
    }

    MIXX_FORCEINLINE list_iterator& operator++(int) {
        list_iterator tmp(*this);
        _p = _p->next;
        return tmp;
    }

    MIXX_FORCEINLINE value_type& operator*() {
        return _p->value;
    }

    MIXX_FORCEINLINE bool operator==(list_iterator const& rhs) {
        return _p == rhs._p;
    }

    MIXX_FORCEINLINE bool operator!=(list_iterator const& rhs) {
        return _p != rhs._p;
    }

    MIXX_FORCEINLINE node_type* node() const {
        return _p;
    }
    
private:
    node_type* _p;
};

template <typename T, typename Allocator = std::allocator<T>>
class list: noncopyable {
    using node_type = __list_node<T>;
    using node_pointer = node_type*;
public:
    using value_type = T;
    using alloc_type = Allocator;
    using iterator = list_iterator<T>;

    explicit list(alloc_type const& alloc = alloc_type());
    ~list() noexcept;
    template <typename ...Args> iterator emplace_back_to(iterator const& target, Args&& ...args);
    template <typename ...Args> iterator emplace_front_of(iterator const& target, Args&& ...args);
    template <typename ...Args> iterator emplace_front(Args&& ...args);
    template <typename ...Args> iterator emplace_back(Args&& ...args);
    iterator remove_node(iterator const& target) noexcept;
    void clear();

    MIXX_FORCEINLINE iterator begin() const {
        return iterator{ _head->next };
    }

    MIXX_FORCEINLINE iterator tail() const {
        return iterator{ _tail };
    }

    MIXX_FORCEINLINE iterator end() const {
        return iterator{ nullptr };
    }

private:
    template <typename ...Args>
    static MIXX_FORCEINLINE void _init_value(node_type* node, Args ...args) {
        new (&node->value) value_type(std::forward<Args>(args)...);
    }

    static MIXX_FORCEINLINE void _free_value(node_type* node) {
        node->value.~value_type();
    }

    static node_type* _allocate_node();
    static void _deallocate_node(node_type* node) noexcept;

    using alproxy_t = typename std::allocator_traits<alloc_type>::template rebind_alloc<node_type>;
    alproxy_t   _alproxy;
    node_pointer _head, _tail;
    mixx_size_t _size;
};

template <typename T, typename Allocator>
list<T, Allocator>::list(alloc_type const& alloc): _alproxy(alloc) {
    _tail = _head = _allocate_node();
    _head->prev = nullptr;
    _head->next = nullptr;
    _size = 0;
}

template <typename T, typename Allocator>
list<T, Allocator>::~list() noexcept {
    clear();
    _deallocate_node(_head);
}

template <typename T, typename Allocator>
template <typename ... Args>
typename list<T, Allocator>::iterator list<T, Allocator>::emplace_back_to(iterator const& target, Args&&... args) {
    node_pointer dest = target.node(), next = dest->next, node = _allocate_node();
    _init_value(node, std::forward<Args>(args)...);
    node->prev = dest; node->next = next;
    dest->next = node;
    if (next != nullptr)
        next->prev = node;
    else
        _tail = node;
    _size += 1;
    return iterator(node);
}

template <typename T, typename Allocator>
template <typename ... Args>
typename list<T, Allocator>::iterator list<T, Allocator>::emplace_front_of(iterator const& target, Args&&... args) {
    node_pointer dest = target.node(), prev = dest->prev, node = _allocate_node();
    _init_value(node, std::forward<Args>(args)...);
    node->prev = prev; node->next = dest;
    dest->prev = node;
    prev->next = node;
    _size -= 1;
    return iterator(node);
}

template <typename T, typename Allocator>
template <typename ... Args>
typename list<T, Allocator>::iterator list<T, Allocator>::emplace_front(Args&&... args) {
    return emplace_back_to(iterator(_head), std::forward<Args>(args)...);
}

template <typename T, typename Allocator>
template <typename ... Args>
typename list<T, Allocator>::iterator list<T, Allocator>::emplace_back(Args&&... args) {
    return emplace_back_to(iterator(_tail), std::forward<Args>(args)...);
}

template <typename T, typename Allocator>
typename list<T, Allocator>::iterator list<T, Allocator>::remove_node(iterator const& target) noexcept {
    node_pointer dest = target.node(), prev = dest->prev, next = dest->next;
    _free_value(dest);
    prev->next = next;
    if (next != nullptr)
        next->prev = prev;
    else
        _tail = prev;
    _deallocate_node(dest);
    return iterator(next);
}

template <typename T, typename Allocator>
void list<T, Allocator>::clear() {
    while (_head->next != nullptr)
        remove_node(iterator(_head->next));
}

template <typename T, typename Allocator>
typename list<T, Allocator>::node_type* list<T, Allocator>::_allocate_node() {
    return static_cast<node_pointer>(malloc(sizeof(node_type)));
}

template <typename T, typename Allocator>
void list<T, Allocator>::_deallocate_node(node_type* node) noexcept {
    free(node);
}

} // !namespace mixx

#endif // _MIXX_H_LIST_H_
