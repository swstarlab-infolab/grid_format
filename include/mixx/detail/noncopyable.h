#ifndef _MIXX_DETAIL_NONCOPYABLE_H_
#define _MIXX_DETAIL_NONCOPYABLE_H_

namespace mixx {

namespace _noncopyable { // protection from unintended ADL

class noncopyable {
public:
    noncopyable(const noncopyable&) = delete;
    noncopyable& operator=(const noncopyable&) = delete;
protected:
    /*constexpr*/ noncopyable() = default;
    ~noncopyable() = default;
};
} // !namespace _noncopyable

using noncopyable = _noncopyable::noncopyable;

} // !namespace mixx

#endif // !_MIXX_DETAIL_NONCOPYABLE_H_