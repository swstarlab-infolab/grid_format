#include <gstream/grid_format/grid_generator_defines.h>
#include <gstream/error.h>

namespace gstream {
namespace grid_format {
namespace _generator {

void gb_bulding_block::set_gbid(gbid_t const& gbid) {
    header.gbid = gbid;
}

void gb_bulding_block::add_segment(seg_id_t sid, local_vid row_id, local_vid* adj, size_t len) {
    assert(col16.size() <= std::numeric_limits<segptr_t>::max());
    segptr.push_back(static_cast<segptr_t>(col16.size()));
    for (size_t i = 0; i < len; ++i)
        col16.push_back(adj[i]);
    segidx.emplace_back(sid, row24_t { row_id });
}

size_t gb_bulding_block::block_size() const {
    size_t s = sizeof(gb_header_t);
    s += sizeof(col16_t) * col16.size();
    s += sizeof(segptr_t) * segptr.size();
    s += sizeof(segidx_t) * segidx.size();
    return s;
}

bool gb_bulding_block::serialize(FILE* os) {
    if (1 != fwrite(&header, sizeof(header), 1, os))
        goto lb_err;
    if (col16.size() != fwrite(col16.data(), sizeof(col16_t), col16.size(), os))
        goto lb_err;
    if (segptr.size() != fwrite(segptr.data(), sizeof(segptr_t), segptr.size(), os))
        goto lb_err;
    if (segidx.size() != fwrite(segidx.data(), sizeof(segidx_t), segidx.size(), os))
        goto lb_err;
    
    return true;

lb_err:
    _GSTREAM_TLS_ERROR_LOG(GStreamFileWriteError);
    return false;
}

} // !namespace _generator
} // !namespace grid_format
} // !namespace gstream