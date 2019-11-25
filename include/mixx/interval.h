#ifndef _MIXX_INTERVAL_H_
#define _MIXX_INTERVAL_H_

namespace mixx {

template <typename T>
class closed_interval {
public:
    using value_type = T;

    class iterator {
    public:
        explicit iterator(value_type start) {
            _i = start;
        }

        value_type operator*() const {
            return _i;
        }

        const iterator& operator++() {
            ++_i;
            return *this;
        }

        iterator operator++(int) {
            iterator c(*this);
            ++_i;
            return c;
        }

        bool operator==(const iterator& other) const {
            return _i == other._i;
        }
        bool operator!=(const iterator& other) const {
            return _i != other._i;
        }

    private:
        value_type _i;
    };

    closed_interval() {
        _a = 0;
        _b = 0;
    }

    ~closed_interval() noexcept = default;

    closed_interval(value_type min, value_type max) {
        _a = min;
        _b = max;
    }

    iterator begin() const {
        return _a;
    }

    iterator end() const {
        return _b;
    }

    value_type min() const {
        return _a;
    }

    value_type max() const {
        return _b;
    }

    void set(value_type min, value_type max) {
        _a = min;
        _b = max;
    }

    size_t size() const {
        return static_cast<size_t>(_b) + 1 - _a;
    }

private:
    value_type _a;
    value_type _b;
};

template <typename T>
using cintvl_t = closed_interval<T>;

} // !namespace mixx

#endif // _MIXX_INTERVAL_H_
