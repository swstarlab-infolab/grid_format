#ifndef _GSTREAM_GRID_FORMAT_GBB_BUFFER_H_
#define _GSTREAM_GRID_FORMAT_GBB_BUFFER_H_
#include <gstream/grid_format/grid_generator_defines.h>
#include <mixx/matrix.h>

namespace gstream {
namespace grid_format {
namespace _generator {

class mm_gbb_buffer final {
public:
    gb_bulding_block& acquire_gbb(gbid_t const& gbid);
    grid_dim_t dim() const;
    void clear();
private:
    mixx::matrix2<gb_bulding_block> _gbb_mat;
};

} // !namespace _generator
} // !namespace grid_format
} // !namespace gstream

#endif // !_GSTREAM_GRID_FORMAT_GBB_BUFFER_H_
