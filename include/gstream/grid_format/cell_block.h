/*!
 * GStream project by InfoLab @ DGIST (https://infolab.dgist.ac.kr)
 * Copyright 2019 GStream Authors. All Rights Reserved.
 * 
 * <License>
 * 
 * @file gstream/grid_format/cell_block.h
 * @project LibGStream
 * @brief Definition of a cell block, which is part of the Grid format
 * @author Seyeon Oh (mailto:vee@dgist.ac.kr)
 * @version 1.0 4/25/2019
 */

#ifndef _GSTREAM_GRID_FORMAT_CELL_BLOCK_H_
#define _GSTREAM_GRID_FORMAT_CELL_BLOCK_H_
#include <gstream/grid_format/internal_defines.h>
#include <mixx/detail/noncopyable.h>
#include <mixx/size.h>

/**
 * Cell block
 * - Container for encoded edges of the corresponding grid block
 * - Variable size container (max. size = slotted page size)
 * 
 * Graph data representation
 * - Encode edge set E(u,v) to 2-byte local edge set E`(u`,v`)
 *     - u`: a row index of grid block
 *     - v`: a column index of grid block
 * - Store own edge set based on Compressed Sparse Column (CSC) like format
 *     - Cols: a set of u` of E`
 *     - Rows: a set of v` of u`
 *     - Colptrs: a set of pointers to map u` and set of v`
 * - Store own edge set in the column manner
 * 
 * Example:
 * +---------------------+-----------------------------------------------------------+
 * |      <Grid G>       |  <Cell blocks of G, where Grid width = 8, Page size = 10> |
 * |   0 1 2 3 4 5 6 7   |         <Cell block 0 of G>                               |
 * |  +---------------+  |  +-------------------------------+                        |
 * | 0|  #   #   #    |  |  | Cols    |0|1|                 |                        |
 * | 1|  #   #        |  |  | Colptrs |0|5|                 |                        |
 * | 2|# #         #  |  |  |         (0)       (5)         |                        |
 * | 3|# #     #   #  |  |  | Rows    |2|3|4|5|6|0|1|2|3|4| |                        |
 * | 4|# #     #   #  |  |  +-------------------------------+                        |
 * | 5|#       #      |  |         <Cell block 1 of G>           <Cell block 2 of G> |
 * | 6|#   #   #      |  |  +-------------------------------+    +-----------------+ |
 * | 7|    #   #      |  |  | Cols    |2|3|4|5|             |    | Cols    |6|     | |
 * |  +---------------+  |  | Colptrs |0|2|4|9|             |    | Colptrs |0|     | |
 * |                     |  |         (0) (2) (4)       (9) |    |         (0)     | |
 * |                     |  | Rows    |6|7|0|1|3|4|5|6|7|0| |    | Rows    |2|3|4| | |
 * |                     |  +-------------------------------+    +-----------------+ |
 * +---------------------+-----------------------------------------------------------+
 * 
 * Physical data layout:
 * +----------------------------+
 * |           Header           |
 * +----------------------------+
 * | Colptr (4-byte section)    |
 * +----------------------------+    
 * |                            |
 * |    Rows (2-byte section)   |
 * |                            |
 * +----------------------------+    
 * |    Cols (2-byte section)   |
 * +----------------------------+
 */

namespace gstream {
namespace _grid_format {

#pragma pack(push, 4)

struct cell_block_header {
    gbid_t gbid; // grid block ID
    cbid_t cbid; // cell block ID is an internal offset of grid block
    pid_t pid; // page ID
    struct {
        l1_size_t physical;
        l1_size_t ptrs;
        l1_size_t rows;
        l1_size_t cols;
    } offset;
    l1_size_t column_count;
    l1_size_t edge_count;
};

class cell_block_column {
public:
    encoded_vid* rows;
    encoded_vid  colidx;
    l1_size_t length;

    class iterator {
        friend class cell_block_column;
        using end_type = nullptr_t;
    public:
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE explicit iterator(cell_block_column& col);
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE iterator(cell_block_column& col, end_type const&);
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE iterator& operator++();
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE iterator  operator++(int);
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE iterator& operator+=(l2_size_t n);
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE iterator& operator=(end_type const&);
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE bool operator==(iterator const& other) const;
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE bool operator==(end_type const&) const;
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE bool operator!=(iterator const& other) const;
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE bool operator!=(end_type const&) const;
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE encoded_vid operator*() const;
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE bool is_end() const;
        cell_block_column& _col;
        l1_size_t _ridx;
    };

    MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE iterator begin() {
        return iterator{ *this };
    }

    MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE iterator begin(l2_size_t offset) {
        iterator iter{ *this };
        iter += offset;
        return iter;
    }

    MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE static iterator::end_type end() {
        return nullptr;
    }
};

MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
cell_block_column::iterator::iterator(cell_block_column& col): _col(col) {
    _ridx = 0;
}

MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
cell_block_column::iterator::iterator(cell_block_column& col, end_type const&): _col(col) {
    _ridx = _col.length;
}

MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
cell_block_column::iterator& cell_block_column::iterator::operator=(end_type const&) {
    _ridx = _col.length;
    return *this;
}

MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
bool cell_block_column::iterator::operator==(end_type const&) const {
    return is_end();
}

MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
cell_block_column::iterator& cell_block_column::iterator::operator++() {
    assert(_ridx < _col.length);
    _ridx += 1;
    return *this;
}

MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
cell_block_column::iterator cell_block_column::iterator::operator++(int) {
    iterator const old = *this;
    this->operator++();
    return old;
}

MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
cell_block_column::iterator& cell_block_column::iterator::operator+=(l2_size_t n) {
    assert(_ridx < _col.length);
    l1_size_t const next = _ridx + n;
    _ridx = next < _col.length ? next : _col.length;
    return *this;
}

MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
bool cell_block_column::iterator::operator==(iterator const& other) const {
    return _ridx == other._ridx;
}

MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE
bool cell_block_column::iterator::operator!=(iterator const& other) const {
    return _ridx != other._ridx;
}

MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE
bool cell_block_column::iterator::operator!=(end_type const&) const {
    return !is_end();
}

MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
encoded_vid cell_block_column::iterator::operator*() const {
    assert(_ridx < _col.length);
    return _col.rows[_ridx];
}

inline bool cell_block_column::iterator::is_end() const {
    return _ridx == _col.length;
}

class cell_block: public cell_block_header, mixx::noncopyable {
    // Note: class cell_block is just interface, a size of cell_block is zero
    cell_block() = delete;
    ~cell_block() = delete;
public:
    using colptr_t = uint32_t;
    /* static assertions */
    static_assert(mixx::is_power_of_two(sizeof(encoded_vid)), "a size of encoded_vid must be power of two!");
    static_assert(mixx::is_power_of_two(sizeof(colptr_t)), "a size of colptr_t must be power of two!");
    static_assert(sizeof(colptr_t) % 4 == 0, "a size of colptr_t must be a multiple of alignment!");

    static constexpr l1_size_t dummy_size = sizeof(colptr_t);
    static constexpr l1_size_t minimum_column_data_size = sizeof(colptr_t) + sizeof(encoded_vid) * 2;
    static constexpr l1_size_t minimum_size = sizeof(cell_block_header) + minimum_column_data_size + dummy_size;

    class iterator {
    public:
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE explicit iterator(cell_block& cb);
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE explicit iterator(cell_block& cb, l2_size_t ridx);
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE iterator& operator++();
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE iterator  operator++(int);
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE iterator& operator+=(l2_size_t n);
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE bool operator==(iterator const& other) const;
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE bool operator!=(iterator const& other) const;
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE cell_block_column operator*() const;
        cell_block& _cb;
        l2_size_t _ridx;
    };

    GSTREAM_DEVICE_COMPATIBLE char* data(l1_size_t off = 0);
    GSTREAM_DEVICE_COMPATIBLE encoded_vid* cols();
    GSTREAM_DEVICE_COMPATIBLE colptr_t* ptrs();
    GSTREAM_DEVICE_COMPATIBLE encoded_vid* rows();
    GSTREAM_DEVICE_COMPATIBLE encoded_vid get_colidx(l2_size_t roff);
    GSTREAM_DEVICE_COMPATIBLE encoded_vid* get_rows(l2_size_t roff);
    GSTREAM_DEVICE_COMPATIBLE l2_size_t get_rows_length(l2_size_t roff);
    GSTREAM_DEVICE_COMPATIBLE iterator begin();
    GSTREAM_DEVICE_COMPATIBLE iterator end();
};

MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
cell_block::iterator::iterator(cell_block& cb): _cb(cb) {
    _ridx = 0;
}

MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
cell_block::iterator::iterator(cell_block& cb, l2_size_t ridx): _cb(cb), _ridx(ridx) {
}

MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
cell_block::iterator& cell_block::iterator::operator++() {
    assert(_ridx < _cb.column_count);
    _ridx += 1;
    return *this;
}

MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
cell_block::iterator cell_block::iterator::operator++(int) {
    iterator const old = *this;
    this->operator++();
    return old;
}

MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
cell_block::iterator& cell_block::iterator::operator+=(l2_size_t n) {
    assert(_ridx < _cb.column_count);
    l1_size_t const next = static_cast<l1_size_t>(_ridx) + n;
    _ridx = next < _cb.column_count ? static_cast<l2_size_t>(next) : static_cast<l2_size_t>(_cb.column_count);
    return *this;
}

MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
bool cell_block::iterator::operator==(iterator const& other) const {
    return _ridx == other._ridx;
}

MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
bool cell_block::iterator::operator!=(iterator const& other) const {
    return _ridx != other._ridx;
}

MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
cell_block_column cell_block::iterator::operator*() const {
    assert(_ridx < _cb.column_count);
    cell_block_column col;
    col.colidx = _cb.get_colidx(_ridx);
    col.rows   = _cb.get_rows(_ridx);
    col.length = _cb.get_rows_length(_ridx);
    return col;
}

MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
char* cell_block::data(l1_size_t const off) {
    return reinterpret_cast<char*>(this) + sizeof(cell_block_header) + off;
}

MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
encoded_vid* cell_block::cols() {
    encoded_vid* p = reinterpret_cast<encoded_vid*>(data(offset.cols));
    return p;
}

MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
cell_block::colptr_t* cell_block::ptrs() {
    colptr_t* p = reinterpret_cast<colptr_t*>(data(offset.ptrs));
    return p;
}

MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
encoded_vid* cell_block::rows() {
    encoded_vid* p = reinterpret_cast<encoded_vid*>(data(offset.rows));
    return p;
}

MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE
encoded_vid cell_block::get_colidx(l2_size_t roff) {
    assert(roff < column_count);
    return cols()[roff];
}

MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE
encoded_vid* cell_block::get_rows(l2_size_t roff) {
    assert(roff < column_count);
    colptr_t const p = ptrs()[roff];
    return &rows()[p];
}

MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE
l2_size_t cell_block::get_rows_length(l2_size_t roff) {
    assert(roff < column_count);
    colptr_t* p = ptrs();
    return static_cast<l2_size_t>(p[roff + 1] - p[roff]);
}

MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE
cell_block::iterator cell_block::begin() {
    return iterator{ *this, 0 };
}

MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE
cell_block::iterator cell_block::end() {
    return iterator{ *this, static_cast<l2_size_t>(column_count) };
}

#pragma pack(pop) // !4-byte (CUDA) alignment

} // !namespace grid_format
} // !namespace gstream

#endif // !_GSTREAM_GRID_FORMAT_CELL_BLOCK_H_
