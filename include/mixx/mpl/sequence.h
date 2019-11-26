#ifndef _MIXX_MPL_SEQUENCE_H_
#define _MIXX_MPL_SEQUENCE_H_

namespace mixx {
namespace mpl {

template<int ...>
struct sequence_type {

};

template<int N, int ...S>
struct sequence_generator : sequence_generator < N - 1, N - 1, S... > {
};

template<int ...S>
struct sequence_generator < 0, S... > {
    typedef sequence_type<S...> type;
};

} // !namespace mpl
} // !namespace mixx

#endif // _MIXX_MPL_SEQUENCE_H_
