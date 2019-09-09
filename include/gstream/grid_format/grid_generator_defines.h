#ifndef _GSTREAM_GRID_FORMAT_GRID_GENERATOR_DEFINES_H_
#define _GSTREAM_GRID_FORMAT_GRID_GENERATOR_DEFINES_H_
#include <gstream/grid_format/grid_dataset.h>
#include <vector>

namespace gstream {
namespace grid_format {
namespace _generator {

class gb_bulding_block {
public:
    void set_gbid(gbid_t const& gbid);
    void add_segment(seg_id_t sid, local_vid row_id, local_vid* adj, size_t len);
    size_t block_size() const;
    gb_header_t header;
    std::vector<col16_t> col16;
    std::vector<segptr_t> segptr;
    std::vector<segidx_t> segidx;
    bool serialize(FILE* os);
};

} // !namespace _generator
} // !namespace grid_format
} // !namespace gstream

#endif // !_GSTREAM_GRID_FORMAT_GRID_GENERATOR_DEFINES_H_
