#ifndef _MIXX_DIMENSION_H_
#define _MIXX_DIMENSION_H_

namespace mixx {

template <typename ValueTy>
struct dim2 {
    using value_type = ValueTy;
    value_type x;
    value_type y;
    value_type size() const {
        return x * y;
    }
};

template <typename ValueTy>
struct dim3 {
    using value_type = ValueTy;
    value_type x;
    value_type y;
    value_type z;
    value_type size() const {
        return x * y * z;
    }
};

} // !namespace mixx

#endif // !_MIXX_DIMENSION_H_
