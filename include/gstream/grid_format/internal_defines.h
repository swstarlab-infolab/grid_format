#ifndef _GSTREAM_GRID_FORMAT_INTERNAL_DEFINES_H_
#define _GSTREAM_GRID_FORMAT_INTERNAL_DEFINES_H_
#include <mixx/mixx_env.h>
#include <mixx/dimension.h>
#include <assert.h>
#include <limits>

namespace gstream {
namespace grid_format {

using gb_index_t = uint16_t;
constexpr gb_index_t MaxGridIndex = std::numeric_limits<gb_index_t>::max();
using gdim_t = mixx::dim2<gb_index_t>;
using col16_t = uint16_t;
using seg_id_t = uint8_t;
constexpr seg_id_t MaxSegmentID = std::numeric_limits<seg_id_t>::max();
using global_vid = uint64_t;
using local_vid = uint32_t;

using seg_rowinr_t = uint8_t;
constexpr uint32_t MaxSegmentedRowInterval = std::numeric_limits<seg_rowinr_t>::max();
using djct_rowinr_t = uint8_t;
constexpr uint32_t MaxDisjunctRowInterval = std::numeric_limits<djct_rowinr_t>::max();

struct global_edge {
    global_vid src;
    global_vid dst;
};

constexpr uint32_t GridWidth = 1u << 24;
constexpr uint32_t SegmentWidth = 1u << 16;
constexpr uint32_t DenseFactor = 4096;

struct segment_index {
    uint32_t __unused;
    MIXX_FORCEINLINE seg_id_t sid() const {
        return static_cast<seg_id_t>(__unused >> 24);
    }
    MIXX_FORCEINLINE uint32_t row24() const {
        return (0x00FFFFFF & __unused);
    }
};

using seg_flag_t = uint8_t;

struct segment_flag {
    static constexpr seg_flag_t Dense = 0x01;
};

struct segment_pointer {
    uint32_t __unused;
    MIXX_FORCEINLINE seg_flag_t flag() const {
        return static_cast<seg_flag_t>(__unused >> 24);
    }
    MIXX_FORCEINLINE uint32_t offset() const {
        return (0x00FFFFFF & __unused);
    }
};

class gb_point {
public:
    gb_index_t col;
    gb_index_t row;
    using compressed_t = uint32_t;

    gb_point() = default;

    gb_point(gb_index_t const col_, gb_index_t const row_) {
        set(col_, row_);
    }

    compressed_t compressed() const {
        return (row << 16) | col;
    }

    void set(gb_index_t const col_, gb_index_t row_) {
        col = col_; row = row_;
    }

    bool operator<(gb_point const& rhs) const {
        return compressed() < rhs.compressed();
    }

    bool operator>(gb_point const& rhs) const {
        return compressed() > rhs.compressed();
    }

    bool operator==(gb_point const& rhs) const {
        return compressed() == rhs.compressed();
    }
};
static_assert(sizeof(gb_point) == sizeof(gb_point::compressed_t), "a size of gb_point::compressed_t is smaller than gb_point!");

MIXX_FORCEINLINE gb_index_t global_vid_to_gb_index(global_vid vid) {
    assert(vid / GridWidth <= MaxGridIndex);
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

#endif // _GSTREAM_GRID_FORMAT_INTERNAL_DEFINES_H_
