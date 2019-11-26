/*!
 * GStream project by InfoLab @ DGIST (https://infolab.dgist.ac.kr)
 * Copyright 2019 GStream Authors. All Rights Reserved.
 * 
 * <License>
 * 
 * @file gstream/grid_format/grid_block.h
 * @project LibGStream
 * @brief Definition of a grid block, which is part of the Grid format
 * @author Seyeon Oh (mailto:vee@dgist.ac.kr)
 * @version 1.0 4/25/2019
 */

#ifndef _GSTREAM_GRID_FORMAT_GRID_BLOCK_H_
#define _GSTREAM_GRID_FORMAT_GRID_BLOCK_H_
#include <gstream/grid_format/gbi.h>
#include <gstream/grid_format/grid_page.h>
#include <gstream/grid_format/cell_block.h>
#include <mixx/detail/noncopyable.h>

namespace gstream {

namespace _grid_format {

#pragma pack(push, 2)

template <typename GridFormat>
class sparse_grid_block: public gbi_node, mixx::noncopyable {
    sparse_grid_block() = delete;
    ~sparse_grid_block() = delete;

public:
    GSTREAM_ALIAS_GRID_FORMAT_INTERNAL_DEFINES(GridFormat);
    class iterator { 
    public:
        using end_type = nullptr_t;
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE explicit iterator(sparse_grid_block& gb);
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE iterator(sparse_grid_block& gb, end_type const& );
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE iterator& operator++();
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE iterator  operator++(int);
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE iterator& operator+=(l2_size_t n);
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE iterator& operator=(end_type const&);
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE bool operator==(iterator const& other) const;
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE bool operator==(end_type const&) const;
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE bool operator!=(iterator const& other) const;
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE bool operator!=(end_type const&) const;
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE cell_block_column operator*() const;
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE bool is_end() const;

    protected:
        l2_size_t cb_index_internal;
        l2_size_t gb_column_index;
        l1_size_t cb_column_index;
        l1_size_t curr_cb_column_count;
        sparse_grid_block& gb; // 0 or 8
        cb_logical_pointer cb_ptr; // 8
        gb_indirect_pointer_block* indirect; // 8

        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE void _make_begin();
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE void _make_end();
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE void _pick_cell_block_direct(l2_size_t cb_index);
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE void _pick_cell_block_indirect(l2_size_t cb_index);
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE void _fetch_indirect_block();
        MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE void _next_indirect_block();
    };

    MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE iterator begin() {
        iterator iter{ *this };
        return iter;
    }

    MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE iterator begin(l2_size_t offset) {
        iterator iter{ *this };
        iter += offset;
        return iter;
    }

    static MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
    typename iterator::end_type end() {
        return nullptr;
    }
};

template <typename GridFormat> MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
sparse_grid_block<GridFormat>::iterator::iterator(sparse_grid_block& gb): gb(gb) {
    _make_begin();
}

template <typename GridFormat> MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
sparse_grid_block<GridFormat>::iterator::iterator(sparse_grid_block& gb, end_type const&): gb(gb) {
    _make_end();
}

template <typename GridFormat> MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
typename sparse_grid_block<GridFormat>::iterator& sparse_grid_block<GridFormat>::iterator::operator++() {
    gb_column_index += 1;
    if (gb_column_index >= gb.column_count) {
        // out of bounds
        _make_end();
        return *this;
    }

    // test a column is in the current cell block
    if (cb_column_index + 1u < curr_cb_column_count) {
        cb_column_index += 1;
        assert(cb_ptr->column_count > cb_column_index);
        return *this;
    }

    // indirect block
    if (indirect != nullptr) {
        if (cb_index_internal + 1u < IndirectBlockSize) {
            // iterate the current indirect block
            _pick_cell_block_indirect(cb_index_internal + 1);
        }
        else {
            // the end of current indirect block; fetch a next block
            _next_indirect_block();
            _pick_cell_block_indirect(0);
        }
    }
    // direct block
    else {
        if (cb_index_internal + 1u < DirectBlockSize) {
            // iterate the direct block
            _pick_cell_block_direct(cb_index_internal + 1);
        }
        else {
            // the end of direct block; fetch an indirect block
            _fetch_indirect_block();
            _pick_cell_block_indirect(0);
        }
    }
    return *this;
}

template <typename GridFormat> MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
typename sparse_grid_block<GridFormat>::iterator sparse_grid_block<GridFormat>::
iterator::operator++(int) {
    iterator const old = *this;
    this->operator++();
    return old;
}

template <typename GridFormat> MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
typename sparse_grid_block<GridFormat>::iterator& sparse_grid_block<GridFormat>::iterator::operator+=(l2_size_t const n) {
    assert(gb_column_index < gb.column_count);

    // boundary checking (grid block level)
    {
        l1_size_t next = static_cast<l1_size_t>(gb_column_index) + n; 
        if (next >= gb.column_count) {
            // out of bounds
            _make_end();
            return *this;
        }
        gb_column_index = static_cast<l2_size_t>(next);
    }

    // test a column is in the current cell block
    if (cb_column_index + n < curr_cb_column_count) {
        cb_column_index += n;
        return *this;
    }

    bool found = false;
    do {
        // indirect block
        if (indirect != nullptr) {
            // find an indirect block containing the column we want
            while (gb_column_index >= indirect->max_agg_column() + 1) {
                _next_indirect_block();
            }

            // find a cell block containing the column we want
            for (l2_size_t i = cb_index_internal + 1u; i < IndirectBlockSize; ++i) {
                if (gb_column_index < indirect->agg_col[i] + 1) {
                    _pick_cell_block_indirect(i);
                    cb_column_index = gb_column_index - (indirect->agg_col[i] + 1 - curr_cb_column_count);
                    found = true;
                    break;
                }
                //assert(i != IndirectBlockSize - 1);
            }
        }
        // direct block
        else {
            // test the column does not exist in the direct block
            if (gb_column_index >= gb.direct.max_agg_column() + 1) {
                _fetch_indirect_block();
                if (gb_column_index < indirect->agg_col[0] + 1) {
                    _pick_cell_block_indirect(0);
                    cb_column_index = gb_column_index - (indirect->agg_col[0] + 1 - curr_cb_column_count);
                    found = true;
                }
                continue; // jump to the indirect block traversal
            }

            // find a cell block containing the column we want
            for (l2_size_t i = cb_index_internal + 1u; i < DirectBlockSize; ++i) {
                if (gb_column_index < gb.direct.agg_col[i] + 1) {
                    _pick_cell_block_direct(i);
                    cb_column_index = gb_column_index - (gb.direct.agg_col[i] + 1 - curr_cb_column_count);
                    found = true;
                    break;
                }
                assert(i != DirectBlockSize - 1);
            }
        }
    } while (!found);

    return *this;
}

template <typename GridFormat> MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
typename sparse_grid_block<GridFormat>::iterator& sparse_grid_block<GridFormat>::iterator::operator=(end_type const&) {
    _make_end();
    return *this;
}

template <typename GridFormat> MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
bool sparse_grid_block<GridFormat>::iterator::operator==(iterator const& other) const {
    return cb_ptr == other.cb_ptr;
}

template <typename GridFormat> MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
bool sparse_grid_block<GridFormat>::iterator::operator==(end_type const&) const {
    return is_end();
}

template <typename GridFormat> MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
bool sparse_grid_block<GridFormat>::iterator::operator!=(iterator const& other) const {
    return cb_ptr != other.cb_ptr;
}

template <typename GridFormat> MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
bool sparse_grid_block<GridFormat>::iterator::operator!=(end_type const&) const {
    return !is_end();
}

template <typename GridFormat> MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
cell_block_column sparse_grid_block<GridFormat>::iterator::operator*() const {
    cell_block_column col;
    col.colidx = cb_ptr->get_colidx(static_cast<l2_size_t>(cb_column_index));
    col.rows = cb_ptr->get_rows(static_cast<l2_size_t>(cb_column_index));
    col.length = cb_ptr->get_rows_length(static_cast<l2_size_t>(cb_column_index));
    return col;
}

template <typename GridFormat> MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
bool sparse_grid_block<GridFormat>::iterator::is_end() const {
    return cb_ptr == nullptr;
}

template <typename GridFormat> MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
void sparse_grid_block<GridFormat>::iterator::_make_begin() {
    assert(gb.cb_count > 0);
    cb_index_internal = 0;
    cb_column_index = 0;
    gb_column_index = 0;
    curr_cb_column_count = gb.direct.col_count[0];
    cb_ptr = gb.direct.ptr[0];
    indirect = nullptr;
}

template <typename GridFormat> MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
void sparse_grid_block<GridFormat>::iterator::_make_end() {
    cb_index_internal = UINT16_MAX;
    cb_column_index = UINT16_MAX;
    gb_column_index = UINT16_MAX;
    curr_cb_column_count = UINT16_MAX;
    cb_ptr = nullptr; // 'end' marking
    indirect = nullptr;
}

template <typename GridFormat> MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
void sparse_grid_block<GridFormat>::iterator::_pick_cell_block_direct(l2_size_t cb_index) {
    cb_index_internal = cb_index;
    cb_column_index = 0;
    curr_cb_column_count = gb.direct.col_count[cb_index];
    cb_ptr = gb.direct.ptr[cb_index];
}

template <typename GridFormat> MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
void sparse_grid_block<GridFormat>::iterator::_pick_cell_block_indirect(l2_size_t cb_index) {
    cb_index_internal = cb_index;
    cb_column_index = 0;
    curr_cb_column_count = indirect->col_count[cb_index];
    cb_ptr = indirect->ptr[cb_index];
}

template <typename GridFormat> MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
void sparse_grid_block<GridFormat>::iterator::_fetch_indirect_block() {
    indirect = gb.indirect;
}

template <typename GridFormat> MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
void sparse_grid_block<GridFormat>::iterator::_next_indirect_block() {
    indirect = indirect->next;
}

#pragma pack(pop) // !2-byte align

} // !namespace grid_format

} // !namespace gstream

#endif // !_GSTREAM_GRID_FORMAT_GRID_BLOCK_H_
