#ifndef _MIXX_H_LIST_H_
#define _MIXX_H_LIST_H_
#include <mixx/detail/list_base.h>
#include <mixx/detail/noncopyable.h>
#include <memory>

namespace mixx {

template <typename T, typename Allocator = std::allocator<T>>
class list : noncopyable {
    using node_type = list_impl::list_node<T>;
    using node_pointer = node_type*;
public:
    using value_type = T;
    using alloc_type = Allocator;
    using iterator = list_impl::list_iterator<T>;

    explicit list(alloc_type const& alloc = alloc_type());
    ~list() noexcept;
    template <typename ...Args> iterator emplace_front_of(iterator const& target, Args&& ...args);
    template <typename ...Args> iterator emplace_back_to(iterator const& target, Args&& ...args);
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

    MIXX_FORCEINLINE bool empty() const {
        return _head->next == nullptr;
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
list<T, Allocator>::list(alloc_type const& alloc) : _alproxy(alloc) {
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
typename list<T, Allocator>::iterator list<T, Allocator>::emplace_front_of(iterator const& target, Args&&... args) {
    return list_impl::emplace_front_of(*this, target, std::forward<Args>(args)...);
}

template <typename T, typename Allocator>
template <typename ... Args>
typename list<T, Allocator>::iterator list<T, Allocator>::emplace_back_to(iterator const& target, Args&&... args) {
    return list_impl::emplace_back_to(*this, target, std::forward<Args>(args)...);
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
    return list_impl::remove_node(*this, target);
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
