/*!
 * GStream project by InfoLab @ DGIST (https://infolab.dgist.ac.kr)
 * Copyright 2019 GStream Authors. All Rights Reserved.
 * 
 * <License>
 * 
 * @file gstream/grid_format/internal_defines.h
 * @project LibGStream
 * @brief Internal typedefs for Grid format 
 * @author Seyeon Oh (mailto:vee@dgist.ac.kr)
 * @version 1.0 4/25/2019
 */
#ifndef _GSTREAM_GRID_FORMAT_INTERNAL_DEFINES_H_
#define _GSTREAM_GRID_FORMAT_INTERNAL_DEFINES_H_
#include <gstream/cuda_env.h>
#include <mixx/dimension.h>
#include <mixx/bitset.h>

namespace gstream {
namespace _grid_format {

using gb_axis_t = uint32_t;
constexpr uint64_t InvalidGridPoint = UINT64_MAX; 
#pragma pack(push, 4)

using encoded_vid = uint16_t;
using pid_t = uint32_t;
using cbid_t = uint16_t;
using l0_size_t = uint64_t;
using l1_size_t = uint32_t; 
using l2_size_t = uint16_t;
using cb_offset_t = uint32_t;

using grid_dim_t = mixx::dim2<l1_size_t>;

typedef struct grid_block_id {
    gb_axis_t col; 
    gb_axis_t row;

    grid_block_id() = default;
    grid_block_id(grid_block_id const& other) = default;
    ~grid_block_id() = default;

    MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
    uint64_t& compressed() {
        return *reinterpret_cast<uint64_t*>(this);
    }

    MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
    uint64_t const& compressed() const {
        return *reinterpret_cast<uint64_t const*>(this);
    }

    MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE
    uint64_t row_major_offset(grid_dim_t const& dim) const {
        return (dim.x * row) + col;
    }

    MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE
    uint64_t column_major_offset(grid_dim_t const& dim) const {
        return (dim.y * col) + row;
    }

    MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
    void set(gb_axis_t col_, gb_axis_t row_) {
        col = col_;
        row = row_;
    }

    MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
    void set_invalid() {
        compressed() = InvalidGridPoint;
    }

    MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
    bool is_invalid() const {
        return compressed() == InvalidGridPoint;
    }

    MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
    bool is_valid() const {
        return compressed() != InvalidGridPoint;
    }

    MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE
    bool operator<(const grid_block_id& other) const {
        return compressed() < other.compressed();
    }

    MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE
    bool operator==(const grid_block_id& other) const {
        return compressed() == other.compressed();
    }

     MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE
    bool operator>(const grid_block_id& other) const {
        return compressed() > other.compressed();
    }
} gbid_t;

static_assert(sizeof(gbid_t) == 8, "a size of grid_point is wrong");

#pragma pack(pop)

#pragma pack(push, 2)
struct encoded_edge {
    encoded_vid src;
    encoded_vid dst;
    bool operator==(encoded_edge const& rhs) const {
        return src == rhs.src && dst == rhs.dst;
    }
};
#pragma pack(pop) // !2-byte alignment

static_assert(sizeof(encoded_edge) == 4, "invliad encoded edge size!");

struct global_edge {
    l0_size_t src;
    l0_size_t dst;
};

MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
bool operator==(global_edge const& lhs, global_edge const& rhs) {
    return lhs.dst == rhs.dst && lhs.src == rhs.src;
}

MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE 
bool operator!=(global_edge const& lhs, global_edge const& rhs) {
    return lhs.dst != rhs.dst && lhs.src != rhs.src;
}

template <typename GridFormat>
l1_size_t vid_to_gb_index(l0_size_t vid) {
    using grid_type = GridFormat;
    return static_cast<l1_size_t>(vid / grid_type::gb_width);
}

template <typename GridFormat>
gbid_t edge_to_gbid(global_edge const& edge) {
    gbid_t id;
    id.col = vid_to_gb_index<GridFormat>(edge.dst);
    id.row = vid_to_gb_index<GridFormat>(edge.src);
    return id;
}

class cell_block;

/* Page */
#define GSTREAM_PAGE_TEMPLATE_ARGS \
    l1_size_t PageSize, uint8_t Alignment

#define GSTREAM_PAGE_TEMPLATE template <GSTREAM_PAGE_TEMPLATE_ARGS>

#define GSTREAM_PAGE_TEMPLATE_ARGS_LIST \
    PageSize, Alignment

#define GSTREAM_ALIAS_PAGE_TEMPLATE_ARGS \
    static constexpr l1_size_t page_size = PageSize;\
    static constexpr uint8_t alignment = Alignment

/* Grid block */
#define GSTREAM_GRID_FORMAT_TEMPLATE_ARGS \
    l1_size_t GridWidth, GSTREAM_PAGE_TEMPLATE_ARGS

#define GSTREAM_GRID_FORMAT_TEMPLATE_ARGS_W_DEFAULT_PARAMS \
    l1_size_t GridWidth, l1_size_t PageSize, uint8_t Alignment = 4

#define GSTREAM_GRID_FORMAT_TEMPLATE \
    template <GSTREAM_GRID_FORMAT_TEMPLATE_ARGS>

#define GSTREAM_GRID_FORMAT_TEMPLATE_W_DEFAULT_PARAMS \
    template <GSTREAM_GRID_FORMAT_TEMPLATE_ARGS_W_DEFAULT_PARAMS>

#define GSTREAM_GRID_FORMAT_TEMPLATE_ARGS_LIST \
    GridWidth, GSTREAM_PAGE_TEMPLATE_ARGS_LIST

#define GSTREAM_ALIAS_GRID_FORMAT_TEMPLATE_ARGS \
    GSTREAM_ALIAS_PAGE_TEMPLATE_ARGS;\
    static constexpr l1_size_t gb_width = GridWidth;\
    static constexpr l0_size_t gb_bitmap_size = gb_width * (gb_width / 8)

#define GSTREAM_ALIAS_GRID_FORMAT_INTERNAL_DEFINES(GridFormat) \
    using grid_format = GridFormat;\
    static constexpr l1_size_t gb_width = grid_format::gb_width; \
    static constexpr l0_size_t gb_bitmap_size = grid_format::gb_bitmap_size; \
    static constexpr l1_size_t page_size = grid_format::page_size; \
    static constexpr uint8_t alignment = grid_format::alignment; \
    using gb_edge_bitmap = mixx::bitset<gb_width>

struct gbi_node;

} // !namespace _grid_format
} // !namespace gstream

#endif // !_GSTREAM_GRID_FORMAT_INTERNAL_DEFINES_H_
