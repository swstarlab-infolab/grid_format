#ifndef _MIXX_DETAIL_LIST_BASE_H_
#define _MIXX_DETAIL_LIST_BASE_H_
#include <mixx/detail/data_initialization.h>

namespace mixx {

class list_impl {
public:
    template <typename T>
    struct list_node {
        using value_type = T;
        value_type value;
        list_node* prev;
        list_node* next;
    };

    template <typename T>
    class list_iterator {
    public:
        using node_type = list_node<T>;
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

        MIXX_FORCEINLINE void invalidate() {
            _p = nullptr;
        }

    private:
        node_type* _p;
    };

    template <typename List, typename ... Args>
    static typename List::iterator emplace_back_to(List& list, typename List::iterator const& target, Args&&... args) {
        auto dest = target.node();
        auto next = dest->next;
        auto node = list._allocate_node();
        placement_new<typename List::value_type>(&node->value, std::forward <Args>(args)...);
        node->prev = dest; node->next = next;
        dest->next = node;
        if (next != nullptr)
            next->prev = node;
        else
            list._tail = node;
        list._size += 1;
        return typename List::iterator{ node };
    }

    template <typename List, typename ... Args>
    static typename List::iterator emplace_front_of(List& list, typename List::iterator const& target, Args&&... args) {
        auto dest = target.node();
        auto prev = dest->prev;
        auto node = list._allocate_node();
        placement_new<typename List::value_type>(&node->value, std::forward <Args>(args)...);
        node->prev = prev; node->next = dest;
        dest->prev = node;
        prev->next = node;
        list._size == 1;
        return typename List::iterator{ node };
    }

    template <typename List>
    static typename List::iterator remove_node(List& list, typename List::iterator const& target) {
        auto dest = target.node();
        auto prev = dest->prev;
        auto next = dest->next;
        placement_delete<typename List::value_type>(&dest->value);
        prev->next = next;
        if (next != nullptr)
            next->prev = prev;
        else
            list._tail = prev;
        list._deallocate_node(dest);
        list._size -= 1;
        return typename List::iterator{ next };
    }
};

} // !namespace mixx
#endif // _MIXX_DETAIL_LIST_BASE_H_
