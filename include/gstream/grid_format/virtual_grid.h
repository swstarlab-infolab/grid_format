#ifndef _GSTREAM_GRID_FORMAT_VIRTUAL_GRID_H_
#define _GSTREAM_GRID_FORMAT_VIRTUAL_GRID_H_
#include <gstream/grid_format/grid_block.h>

namespace gstream {
namespace _grid_format {

#define GSTREAM_VIRTUAL_GRID_TEMPLATE_ARGS \
    typename GridFormat, unsigned MaxSize 

#define GSTREAM_VIRTUAL_GRID_TEMPLATE_ARGS_LIST \
    GridFormat, MaxSize

#define GSTREAM_VIRTUAL_GRID_TEMPLATE \
    template <GSTREAM_VIRTUAL_GRID_TEMPLATE_ARGS>

GSTREAM_VIRTUAL_GRID_TEMPLATE
class virtual_grid_block: mixx::noncopyable {

public:
    GSTREAM_ALIAS_GRID_FORMAT_INTERNAL_DEFINES(GridFormat);
    static constexpr unsigned capacity = MaxSize;
    using sparse_gb = sparse_grid_block<grid_format>;
    class iterator {
    public:
        using end_type = nullptr_t;
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE explicit iterator(virtual_grid_block& vgb);
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE iterator(virtual_grid_block& vgb, end_type const&);
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE iterator  operator++(int);
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE iterator  operator++();
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE iterator& operator+=(l1_size_t n);
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE iterator& operator=(end_type const&);
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE bool operator==(iterator const& other) const;
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE bool operator==(end_type const&) const;
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE bool operator!=(iterator const& other) const;
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE bool operator!=(end_type const&) const;
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE sparse_gb* operator*() const;
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE bool is_end() const;

    protected:
        virtual_grid_block& vgb;
        l1_size_t gb_index;

        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE void _make_begin();
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE void _make_end();
    };

    MIXX_FORCEINLINE virtual_grid_block() {
        _size = 0;
        memset(_cont, 0, sizeof(gbi_node*) * capacity);
    }

    MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE iterator begin() {
        iterator iter{ *this };
        return iter;
    }

    MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE iterator begin(l1_size_t offset) {
        iterator iter{ *this };
        iter += offset;
        return iter;
    }

    static MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
    typename iterator::end_type end() {
        return nullptr;
    }

    MIXX_FORCEINLINE unsigned add_gbi(gbi_node* gbi) {
        assert(_size < capacity);
        _cont[_size++] = gbi;
        return _size;
    }

    MIXX_FORCEINLINE void clear() {
        _size = 0;
    }

    MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE unsigned size() const {
        return _size;
    }

    gbi_node* _cont[capacity];
    unsigned _size;
};

#define GSTREAM_VIRTUAL_GRID_BLOCK virtual_grid_block<GridFormat, MaxSize>

GSTREAM_VIRTUAL_GRID_TEMPLATE MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE
GSTREAM_VIRTUAL_GRID_BLOCK::iterator::iterator(virtual_grid_block& vgb): vgb(vgb) {
    _make_begin();
}

GSTREAM_VIRTUAL_GRID_TEMPLATE MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE
GSTREAM_VIRTUAL_GRID_BLOCK::iterator::iterator(virtual_grid_block& vgb, end_type const&): vgb(vgb) {
    _make_end();
}

GSTREAM_VIRTUAL_GRID_TEMPLATE MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE
typename GSTREAM_VIRTUAL_GRID_BLOCK::iterator GSTREAM_VIRTUAL_GRID_BLOCK::
iterator::operator++() {
    assert(gb_index < vgb._size);
    gb_index += 1;
    return *this;
}

GSTREAM_VIRTUAL_GRID_TEMPLATE MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE
typename GSTREAM_VIRTUAL_GRID_BLOCK::iterator GSTREAM_VIRTUAL_GRID_BLOCK::
iterator::operator++(int) {
    iterator const old = *this;
    this->operator++();
    return old;
}

GSTREAM_VIRTUAL_GRID_TEMPLATE MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE
typename GSTREAM_VIRTUAL_GRID_BLOCK::iterator& GSTREAM_VIRTUAL_GRID_BLOCK
::iterator::operator+=(l1_size_t const n) {
    assert(gb_index < vgb._size);
    l1_size_t const next = gb_index + n;
    gb_index = next < vgb._size ? next : vgb._size;
    return *this;
}

GSTREAM_VIRTUAL_GRID_TEMPLATE MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE
typename GSTREAM_VIRTUAL_GRID_BLOCK::iterator& GSTREAM_VIRTUAL_GRID_BLOCK
::iterator::operator=(end_type const&) {
    _make_end();
    return *this;
}

GSTREAM_VIRTUAL_GRID_TEMPLATE MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE
bool GSTREAM_VIRTUAL_GRID_BLOCK::iterator::operator==(iterator const& other) const {
    return gb_index == other.gb_index;
}

GSTREAM_VIRTUAL_GRID_TEMPLATE MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE
bool GSTREAM_VIRTUAL_GRID_BLOCK::iterator::operator==(end_type const&) const {
    return is_end();
}

GSTREAM_VIRTUAL_GRID_TEMPLATE MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE
bool GSTREAM_VIRTUAL_GRID_BLOCK::iterator::operator!=(iterator const& other) const {
    return gb_index != other.gb_index;
}

GSTREAM_VIRTUAL_GRID_TEMPLATE MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE
bool GSTREAM_VIRTUAL_GRID_BLOCK::iterator::operator!=(end_type const&) const {
    return !is_end();
}

GSTREAM_VIRTUAL_GRID_TEMPLATE MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE
typename GSTREAM_VIRTUAL_GRID_BLOCK::sparse_gb* GSTREAM_VIRTUAL_GRID_BLOCK
::iterator::operator*() const {
    return reinterpret_cast<sparse_gb*>(vgb._cont[gb_index]);
}

GSTREAM_VIRTUAL_GRID_TEMPLATE MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE
bool GSTREAM_VIRTUAL_GRID_BLOCK::iterator::is_end() const {
    return gb_index == vgb._size;
}

GSTREAM_VIRTUAL_GRID_TEMPLATE MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE
void GSTREAM_VIRTUAL_GRID_BLOCK::iterator::_make_begin() {
    gb_index = 0;
}

GSTREAM_VIRTUAL_GRID_TEMPLATE MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE
void GSTREAM_VIRTUAL_GRID_BLOCK::iterator::_make_end() {
    gb_index = vgb._size;
}

} // !namespace _grid_format
} // !namespace gstream

#endif // !_GSTREAM_GRID_FORMAT_VIRTUAL_GRID_H_