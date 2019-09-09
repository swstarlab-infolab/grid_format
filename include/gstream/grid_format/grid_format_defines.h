#ifndef _GSTREAM_GRID_FORMAT_DEFINES_H_
#define _GSTREAM_GRID_FORMAT_DEFINES_H_
#include <gstream/cuda_env.h>
#include <mixx/mixx_env.h>
#include <mixx/dimension.h>
#include <assert.h>
#include <limits>

namespace gstream {
namespace grid_format {

// Type define for grid indexing and representation
using gb_index_t = uint32_t; // Grid block index type
constexpr gb_index_t MaxGridBlockIndex = std::numeric_limits<gb_index_t>::max();
using grid_dim_t = mixx::dim2<uint32_t>;
using global_vid = uint64_t;
using local_vid = uint32_t;

using l1_addr_t = uint64_t;
using l2_addr_t = uint32_t;

constexpr uint32_t GridWidth = 1u << 24;
constexpr uint32_t SegmentWidth = 1u << 16;
constexpr uint32_t DenseSegmentFactor = 4096;

template <typename VertexID>
struct edge_template {
    using vid_type = VertexID;
    VertexID src;
    VertexID dst;
};

using global_edge = edge_template<global_vid>;
using local_edge = edge_template<local_vid>;

// Type define for segment indexing and representation
using seg_id_t = uint8_t; // Segment ID type
constexpr seg_id_t MaxSegmentID = std::numeric_limits<seg_id_t>::max();
using col16_t = uint16_t; // Column index (16-bit) type
using loc_row_inr = uint8_t; // Local row interval type
using colptr_t = uint32_t;

struct row24_t {
    GSTREAM_DEVICE_COMPATIBLE MIXX_FORCEINLINE row24_t() = default;
    GSTREAM_DEVICE_COMPATIBLE MIXX_FORCEINLINE ~row24_t() = default;
    GSTREAM_DEVICE_COMPATIBLE MIXX_FORCEINLINE row24_t(row24_t const&) = default;
    GSTREAM_DEVICE_COMPATIBLE MIXX_FORCEINLINE row24_t& operator=(row24_t const& rhs) = default;
    GSTREAM_DEVICE_COMPATIBLE MIXX_FORCEINLINE explicit row24_t(uint32_t value_) {
        value = value_;
    }
    uint32_t value;
};

typedef struct segment_index {
    uint32_t __unused;
    segment_index() = default;
    segment_index(seg_id_t sid, row24_t row24) {
        __unused = sid;
        __unused <<= 24;
        __unused |= row24.value;
    }
    GSTREAM_DEVICE_COMPATIBLE MIXX_FORCEINLINE seg_id_t sid() const {
        return static_cast<seg_id_t>(__unused >> 24);
    }
    GSTREAM_DEVICE_COMPATIBLE MIXX_FORCEINLINE row24_t row24() const {
        return row24_t{ 0x00FFFFFF & __unused };
    }
} segidx_t;

using segment_pointer = uint32_t;
using segptr_t = segment_pointer;

MIXX_FORCEINLINE gb_index_t global_vid_to_gb_index(global_vid vid) {
    assert(vid / GridWidth <= MaxGridBlockIndex);
    return static_cast<gb_index_t>(vid / GridWidth);
}

MIXX_FORCEINLINE local_vid global_vid_to_local_vid(global_vid vid) {
    return static_cast<local_vid>(vid % GridWidth);
}

MIXX_FORCEINLINE seg_id_t local_vid_to_seg_id(local_vid vid) {
    assert(vid / SegmentWidth <= MaxSegmentID);
    return static_cast<seg_id_t>(vid / SegmentWidth);
}

} // !namespace grid_format
} // !namespace gstream

#endif // !_GSTREAM_GRID_FORMAT_DEFINES_H_
