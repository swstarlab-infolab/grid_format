#ifndef _MIXX_POOLING_LIST_H_
#define _MIXX_POOLING_LIST_H_
#include <mixx/detail/list_base.h>
#include <mixx/object_pool.h>

namespace mixx {

template <typename T, mixx_size_t ClusterSize = 256>
using list_node_pool_trait = object_pool< list_impl::list_node<T>, ClusterSize>;

/* class pooling_list */

template <typename T, typename Pool = list_node_pool_trait<T> >
class pooling_list : noncopyable {
    friend class list_impl;
public:
    using node_type = list_impl::list_node<T>;
    using node_pointer = node_type*;
    using value_type = T;
    using pool_type = Pool;
    using iterator = list_impl::list_iterator<T>;

    explicit pooling_list(pool_type& pool);
    ~pooling_list() noexcept;
    template <typename ...Args> iterator emplace_front_of(iterator const& target, Args&& ...args);
    template <typename ...Args> iterator emplace_back_to(iterator const& target, Args&& ...args);
    template <typename ...Args> iterator emplace_front(Args&& ...args);
    template <typename ...Args> iterator emplace_back(Args&& ...args);
    iterator remove_node(iterator const& target);
    iterator remove_node(node_pointer const& target);
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

    node_type* _allocate_node();
    void _deallocate_node(node_type* node);

    pool_type& _pool;
    node_pointer _head, _tail;
    mixx_size_t _size;
};

template <typename T, typename PoolTy>
pooling_list<T, PoolTy>::pooling_list(pool_type& pool) : _pool(pool) {
    _tail = _head = _allocate_node();
    _head->prev = nullptr;
    _head->next = nullptr;
    _size = 0;
}

template <typename T, typename PoolTy>
pooling_list<T, PoolTy>::~pooling_list() noexcept {
    try {
        clear();
        _deallocate_node(_head);
    }
    catch (...) {
        fprintf(stderr, "Failed to cleanup pooling list!");
        std::abort();
    }
}

template <typename T, typename PoolTy>
template <typename ... Args>
typename pooling_list<T, PoolTy>::iterator pooling_list<T, PoolTy>::
emplace_front_of(iterator const& target, Args&&... args) {
    return list_impl::emplace_front_of(*this, target, std::forward<Args>(args)...);
}

template <typename T, typename PoolTy>
template <typename ... Args>
typename pooling_list<T, PoolTy>::iterator pooling_list<T, PoolTy>::
emplace_back_to(iterator const& target, Args&&... args) {
    return list_impl::emplace_back_to(*this, target, std::forward<Args>(args)...);
}

template <typename T, typename PoolTy>
template <typename ... Args>
typename pooling_list<T, PoolTy>::iterator pooling_list<T, PoolTy>::emplace_front(Args&&... args) {
    return emplace_back_to(iterator(_head), std::forward<Args>(args)...);
}

template <typename T, typename PoolTy>
template <typename ... Args>
typename pooling_list<T, PoolTy>::iterator pooling_list<T, PoolTy>::emplace_back(Args&&... args) {
    return emplace_back_to(iterator(_tail), std::forward<Args>(args)...);
}

template <typename T, typename PoolTy>
typename pooling_list<T, PoolTy>::iterator pooling_list<T, PoolTy>::remove_node(iterator const& target) {
    return list_impl::remove_node(*this, target);
}

template <typename T, typename Pool>
typename pooling_list<T, Pool>::iterator pooling_list<T, Pool>::remove_node(node_pointer const& target) {
    return list_impl::remove_node(*this, iterator{ target });
}

template <typename T, typename PoolTy>
void pooling_list<T, PoolTy>::clear() {
    while (_head->next != nullptr)
        remove_node(iterator(_head->next));
}

template <typename T, typename PoolTy>
typename pooling_list<T, PoolTy>::node_type* pooling_list<T, PoolTy>::_allocate_node() {
    return _pool.allocate();
}

template <typename T, typename PoolTy>
void pooling_list<T, PoolTy>::_deallocate_node(node_type* node) {
    _pool.deallocate(node);
}

} // !namespace mixx

#endif // _MIXX_POOLING_LIST_H_
