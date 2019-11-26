/*!
 * GStream project by InfoLab @ DGIST (https://infolab.dgist.ac.kr)
 * Copyright 2019 GStream Authors. All Rights Reserved.
 * 
 * <License>
 * 
 * @file gstream/grid_format/gbi.h
 * @project LibGStream
 * @brief Grid Block Indexing (GBI)
 * @author Seyeon Oh (mailto:vee@dgist.ac.kr)
 * @version 1.0 4/25/2019
 */

#ifndef _GSTREAM_GRID_FORMAT_GBI_H_
#define _GSTREAM_GRID_FORMAT_GBI_H_
#include <gstream/grid_format/internal_defines.h>
#include <gstream/cuda_env.h>
#include <mixx/mixx_env.h>

namespace gstream {
namespace _grid_format {

using gbi_flag = uint32_t;
constexpr gbi_flag GSTREAM_SPARSE_GRID = 0x01;
constexpr gbi_flag GSTREAM_DENSE_GRID = 0x01 << 1;

#pragma pack(push, 4)

struct cb_physical_pointer {
    pid_t pid;
    uint32_t cb_off;
};

using cb_logical_pointer = cell_block*;

static_assert(sizeof(cb_physical_pointer) == sizeof(cb_logical_pointer), "sizeof cb_physical_ptr must be cb_logical_ptr size");

template <typename Pointer>
struct _cb_pointer_selector;

template <>
struct _cb_pointer_selector<cb_physical_pointer> {
    using type = cb_physical_pointer;
};

template <>
struct _cb_pointer_selector<cb_logical_pointer> {
    using type = cb_logical_pointer;
};

template <typename Pointer>
using cb_pointer_t = typename _cb_pointer_selector<Pointer>::type;

constexpr unsigned DirectBlockSize = 4;
constexpr unsigned IndirectBlockSize = 64;

struct physical_pointer {
    uint32_t cluster_id;
    uint32_t block_id;

    physical_pointer& operator=(physical_pointer const& other) {
        cluster_id = other.cluster_id;
        block_id = other.block_id;
        return *this;
    }

    bool is_valid() const {
        return cluster_id < UINT32_MAX && block_id < UINT32_MAX;
    }

    bool is_invalid() const {
        return cluster_id == UINT32_MAX && block_id == UINT32_MAX;
    }

    void set_invalid() {
        cluster_id = UINT32_MAX;
        block_id = UINT32_MAX;
    }
};

static_assert(sizeof(physical_pointer) == 8, "a size of physical_pointer is not 8!");

inline bool is_valid(physical_pointer const& p) {
    return p.is_valid();
}

inline bool is_invalid(physical_pointer const& p) {
    return p.is_invalid();
}

inline void set_invalid(physical_pointer& p) {
    p.set_invalid();
}

struct gb_direct_pointer_block_physical {
    static constexpr unsigned block_size = DirectBlockSize;
    cb_physical_pointer ptr[DirectBlockSize];
    l1_size_t col_count[DirectBlockSize];
    l2_size_t agg_col[DirectBlockSize];
    unsigned size;
};

struct gb_indirect_pointer_block_physical {
    static constexpr unsigned block_size = IndirectBlockSize;
    unsigned size;
    cb_physical_pointer ptr[IndirectBlockSize];
    l1_size_t col_count[IndirectBlockSize];
    l2_size_t agg_col[IndirectBlockSize];
    physical_pointer next;
};

struct gb_direct_pointer_block {
    static constexpr unsigned block_size = DirectBlockSize;
    cb_logical_pointer ptr[DirectBlockSize];
    l1_size_t col_count[DirectBlockSize];
    l2_size_t agg_col[DirectBlockSize];
    unsigned size;

    GSTREAM_DEVICE_COMPATIBLE l2_size_t max_agg_column() const {
        return agg_col[size - 1];
    }
};

struct gb_indirect_pointer_block {
    static constexpr unsigned block_size = IndirectBlockSize;
    unsigned size;
    cb_logical_pointer ptr[IndirectBlockSize];
    l1_size_t col_count[IndirectBlockSize];
    l2_size_t agg_col[IndirectBlockSize];
    gb_indirect_pointer_block* next;
 
    GSTREAM_DEVICE_COMPATIBLE l2_size_t max_agg_column() const {
        return agg_col[size - 1];
    }
};

struct gbi_node {
    gbid_t id;
    l2_size_t cb_count;
    l2_size_t page_count;
    l2_size_t column_count;
    l2_size_t ipb_count;
    l1_size_t edge_count;
    gbi_flag flags; 
    gb_direct_pointer_block direct;
    gb_indirect_pointer_block* indirect;
    gb_direct_pointer_block_physical direct_p;
    physical_pointer indirect_p;
};

#pragma pack(pop) // !4-byte (CUDA) alignment

} // !namespace grid_format
} // !namespace gstream

#endif // !_GSTREAM_GRID_FORMAT_GBI_H_
