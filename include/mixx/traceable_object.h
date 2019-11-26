#ifndef _MIXX_TRACEABLE_OBJECT_H_
#define _MIXX_TRACEABLE_OBJECT_H_
#include <utility>
#include <atomic>

namespace mixx {

namespace _traceable_object_impl {

template <class CounterTy = size_t>
struct counter_template {
    using counter_t = CounterTy;
    counter_t created = 0;
    counter_t self_constructed = 0;
    counter_t copy_constructed = 0;
    counter_t move_constructed = 0;
    counter_t destroyed = 0;
    counter_t assigned = 0;
    counter_t move_assigned = 0;
    counter_t valid = 0;
};

template <class ValueTy>
class traceable_object_base {
public:
    using value_t = ValueTy;
    traceable_object_base(const traceable_object_base<value_t>& other);
    traceable_object_base(traceable_object_base<value_t>&& other) noexcept;
    ~traceable_object_base() noexcept = default;
    template <class ...ValueCtorArgs>
    explicit traceable_object_base(ValueCtorArgs&& ...args);
    value_t value;
};

template <>
class traceable_object_base<void> {
public:
    using value_t = void;
};

template <class ValueTy>
traceable_object_base<ValueTy>::traceable_object_base(const traceable_object_base<value_t>& other)
    : value { other.value } {
}

template <class ValueTy>
traceable_object_base<ValueTy>::traceable_object_base(traceable_object_base<value_t>&& other) noexcept
    : value { std::move(other.value) } {
}

template <class ValueTy>
template <class ... ValueCtorArgs>
traceable_object_base<ValueTy>::traceable_object_base(ValueCtorArgs&&... args)
    : value { std::forward<ValueCtorArgs>(args)... } {
}

} // !namespcae _traceable_object_impl

using mixx_object_trace_counter = _traceable_object_impl::counter_template<size_t>;
using mixx_atomic_object_trace_counter = _traceable_object_impl::counter_template<std::atomic<size_t>>;

template <class ValueTy, class CounterTy = mixx_object_trace_counter>
class traceable_object: public _traceable_object_impl::traceable_object_base<ValueTy> {
    using base_t = _traceable_object_impl::traceable_object_base<ValueTy>;
public:
    using value_t = ValueTy;
    using counter_t = CounterTy;
    using this_t = traceable_object<ValueTy, CounterTy>;
    using pointer_t = this_t*;
    template <class ...ValueCtorArgs>
    explicit traceable_object(CounterTy* counter, ValueCtorArgs&& ... args);
    traceable_object(const this_t& other);
    traceable_object(this_t&& other) noexcept;
    ~traceable_object() noexcept;
    traceable_object&operator=(const this_t& rhs);
    traceable_object&operator=(this_t&& rhs) noexcept;
    void reset_counter();

    counter_t*get_counter() {
        return counter;
    }

    counter_t* counter;
    bool valid;
};

template <class ValueTy, class CounterTy>
template <class ... ValueCtorArgs>
traceable_object<ValueTy, CounterTy>::traceable_object(CounterTy* counter_, ValueCtorArgs&&... args)
    : base_t { std::forward<ValueCtorArgs>(args)... },
      counter { counter_ } {
    ++(counter->created);
    ++(counter->self_constructed);
    ++(counter->valid);
    valid = true;
}

template <class ValueTy, class CounterTy>
traceable_object<ValueTy, CounterTy>::traceable_object(const this_t& other)
    : base_t { static_cast<const base_t&>(other) },
      counter { other.counter } {
    counter->created += 1;
    counter->copy_constructed += 1;
    counter->valid += 1;
    valid = true;
}

template <class ValueTy, class CounterTy>
traceable_object<ValueTy, CounterTy>::traceable_object(this_t&& other) noexcept
    : base_t { static_cast<base_t&&>(other) },
      counter { std::move(other.counter) } {
    counter->created += 1;
    counter->move_constructed += 1;
    valid = true;
    other.valid = false;
    //other.counter = nullptr;
}

template <class ValueTy, class CounterTy>
traceable_object<ValueTy, CounterTy>::~traceable_object() noexcept {
    counter->destroyed += 1;
    if (valid) {
        --(counter->valid);
    }
}

template <class ValueTy, class CounterTy>
traceable_object<ValueTy, CounterTy>& traceable_object<ValueTy, CounterTy>::operator=(const this_t& rhs) {
    this->value = rhs.value;
    ++(counter->assigned);
    valid = true;
    return *this;
}

template <class ValueTy, class CounterTy>
traceable_object<ValueTy, CounterTy>&traceable_object<ValueTy, CounterTy>::operator=(this_t&& rhs) noexcept {
    this->value = std::move(rhs.value);
    counter = std::move(rhs.counter);
    counter->move_assigned += 1;
    valid = true;
    rhs.valid = false;
    return *this;
}

template <class ValueTy, class CounterTy>
void traceable_object<ValueTy, CounterTy>::reset_counter() {
    counter->created = 0;
    counter->self_constructed = 0;
    counter->copy_constructed = 0;
    counter->move_constructed = 0;
    counter->destroyed = 0;
    counter->assigned = 0;
    counter->move_assigned = 0;
}

} // !namespace mixx

#endif // !_MIXX_TRACEABLE_OBJECT_H_
