#ifndef _GSTREAM_GRID_FORMAT_GRID_BLOCK_H_
#define _GSTREAM_GRID_FORMAT_GRID_BLOCK_H_
#include <gstream/grid_format/grid_format_defines.h>

namespace gstream {
namespace grid_format {

typedef struct gbid_struct {
    gb_index_t col;
    gb_index_t row;

    GSTREAM_DEVICE_COMPATIBLE MIXX_FORCEINLINE
    uint64_t compressed() const {
        return *reinterpret_cast<uint64_t const*>(this);
    }

    GSTREAM_DEVICE_COMPATIBLE MIXX_FORCEINLINE
    bool operator<(gbid_struct const& rhs) const {
        return compressed() < rhs.compressed();
    }

    GSTREAM_DEVICE_COMPATIBLE MIXX_FORCEINLINE
    bool operator>(gbid_struct const& rhs) const {
        return compressed() > rhs.compressed();
    }

    GSTREAM_DEVICE_COMPATIBLE MIXX_FORCEINLINE
        bool operator<=(gbid_struct const& rhs) const {
        return compressed() <= rhs.compressed();
    }

    GSTREAM_DEVICE_COMPATIBLE MIXX_FORCEINLINE
        bool operator>=(gbid_struct const& rhs) const {
        return compressed() >= rhs.compressed();
    }

    GSTREAM_DEVICE_COMPATIBLE MIXX_FORCEINLINE
        bool operator==(gbid_struct const& rhs) const {
        return compressed() == rhs.compressed();
    }

    MIXX_FORCEINLINE void set(gb_index_t col_, gb_index_t row_) {
        col = col_;
        row = row_;
    }
} gbid_t;
static_assert(sizeof(gbid_t) == 8, "a size of gbid_struct is not 8");

typedef struct grid_block_header {
    gbid_t gbid;
    uint32_t segptr_off;
    uint32_t segidx_off;
} gb_header_t;

typedef struct segment_struct {
    seg_id_t sid;
    row24_t  row24;
    col16_t* col16;
    uint32_t len;
} segment_t;

/***
 * Grid block physical layout
 * +--------+-----------------------+------------------------+----------------------+
 * | Header | Column indices (Cols) | Segment Pointer Vector | Segment Index Vector |
 * +--------+-----------------------+------------------------+----------------------+
 * ^        ^                       ^                        ^
 * |        |                       |                        |
 * +--Base  +--Data section begin   +--SEGPTR_OFF            +--SEGIDX_OFF
 * 
 * Column index: 16-bit unsigned integer
 * Segment pointer: 32-bit unsigned integer
 * Segment index: 
 * 
 * Segment pointer (32-bit) = | Flag | Column Pointer |
 *                               ^           ^
 *                               |           |
 *                             8-bit       24-bit
 * 
 */

class grid_block {
public:
    GSTREAM_DEVICE_COMPATIBLE MIXX_FORCEINLINE uint64_t seg_count() const {
        return (header.segidx_off - header.segptr_off - sizeof(segment_pointer)) / sizeof(segment_pointer);
    }

    GSTREAM_DEVICE_COMPATIBLE MIXX_FORCEINLINE segment_t segment(uint32_t const i) {
        segment_index const idx = *(segidx() + i);
        segment_pointer const* ptr = segptr();
        segment_t s;
        s.sid = idx.sid();
        s.row24 = idx.row24();
        s.col16 = reinterpret_cast<col16_t*>(data()) + ptr[i];
        s.len = ptr[i + 1] - ptr[i];
        return s;
    }

    GSTREAM_DEVICE_COMPATIBLE MIXX_FORCEINLINE char* data() {
        return reinterpret_cast<char*>(&header + 1);
    }

    GSTREAM_DEVICE_COMPATIBLE MIXX_FORCEINLINE col16_t* col16() {
        return reinterpret_cast<col16_t*>(data());
    }

    GSTREAM_DEVICE_COMPATIBLE MIXX_FORCEINLINE segment_pointer* segptr() {
        return reinterpret_cast<segment_pointer*>(data() + header.segptr_off);
    }

    GSTREAM_DEVICE_COMPATIBLE MIXX_FORCEINLINE segment_index* segidx() {
        return reinterpret_cast<segment_index*>(data() + header.segidx_off);
    }
    
    grid_block_header header;
};

} // !namespace grid_format
} // !namespace gstream

#endif // !_GSTREAM_GRID_FORMAT_GRID_BLOCK_H_
