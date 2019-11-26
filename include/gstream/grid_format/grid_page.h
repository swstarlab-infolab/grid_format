/*!
 * GStream project by InfoLab @ DGIST (https://infolab.dgist.ac.kr)
 * Copyright 2019 GStream Authors. All Rights Reserved.
 * 
 * <License>
 * 
 * @file gstream/grid_format/slotted_page.h
 * @project LibGStream
 * @brief Definition of a slotted page, which is part of the Grid format
 * @author Seyeon Oh (mailto:vee@dgist.ac.kr)
 * @version 1.0 4/25/2019
 */

#ifndef _GSTREAM_GRID_FORMAT_SLOTTED_PAGE_H_
#define _GSTREAM_GRID_FORMAT_SLOTTED_PAGE_H_
#include <gstream/grid_format/cell_block.h>

/**
 * Slotted page is a fixed-size data block that contains one or multiple cell blocks.
 * - Streaming data container (Storage --> System memory --> GPU device memory)
 * - Streaming and caching unit
 * 
 * A size of the page
 * - A size of page can be defined differently per dataset,
 * - We use 1MiB page as the default.
 * 
 * A structure of the page
 * +--------------------------------------+
 * | page header | cell block ...         |
 * +--------------------------------------+
 * |       ...        | cell block   ...  |
 * +--------------------------------------+
 * |  ...  | cell block  ...              |
 * +--------------------------------------+
 * |  ...       |  cell block  |  index   |
 * +--------------------------------------+
 */

namespace gstream {
namespace _grid_format {

#pragma pack(push, 4)

struct grid_page_header {
    pid_t pid; // page ID
    l1_size_t cb_count;
    l1_size_t free;
};

#pragma pack(pop) // !4-byte (CUDA) alignment

GSTREAM_PAGE_TEMPLATE
class grid_page: public grid_page_header, mixx::noncopyable {
public:
    GSTREAM_ALIAS_PAGE_TEMPLATE_ARGS;
    static constexpr l1_size_t data_section_size = page_size - sizeof(grid_page_header);

    grid_page() = default;
    ~grid_page() = default;

    MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE l1_size_t num_cell_blocks() const {
        return cb_count;
    }

    MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE l1_size_t cell_block_index(l1_size_t blk_off) {
        assert(blk_off < cb_count);
        l1_size_t* p = reinterpret_cast<l1_size_t*>(this + 1);
        p -= blk_off;
        return *p;
    }

    MIXX_FORCEINLINE GSTREAM_DEVICE_COMPATIBLE l1_size_t* index() {
        return reinterpret_cast<l1_size_t*>(this + 1) - cb_count;
    }

    char data[page_size - sizeof(grid_page_header)];
};

template <l1_size_t Size>
struct max_free_space_of_page {
    static constexpr l1_size_t value = Size -
        sizeof(grid_page_header) -
        sizeof(l1_size_t); // an entry of index table
};

template <unsigned PageSize>
struct max_grid_block_width {
    static constexpr l1_size_t value = 
        (max_free_space_of_page<PageSize>::value - cell_block::minimum_size + sizeof(encoded_vid)) / sizeof(encoded_vid);
};

inline l1_size_t get_cb_data_offset_of_page(void* page, uint32_t page_size, uint32_t cb_off) {
    l1_size_t* target = static_cast<uint32_t*>(mixx::seek_pointer(page, page_size));
    target -= cb_off + 1;
    return *target;
}

} // !namespace grid_format
} // !namespace gstream

#endif // !_GSTREAM_GRID_FORMAT_SLOTTED_PAGE_H_
