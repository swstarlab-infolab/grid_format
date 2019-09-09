#include <gstream/grid_format/gbb_buffer.h>

namespace gstream {
namespace grid_format {
namespace _generator {

gb_bulding_block& mm_gbb_buffer::acquire_gbb(gbid_t const& gbid) {
    if (gbid.col < _gbb_mat.cols() && gbid.row < _gbb_mat.rows())
        return _gbb_mat.at(gbid.row, gbid.col);
    // reshape required
    size_t const rows =
        (gbid.row < _gbb_mat.rows()) ? 
            _gbb_mat.rows() : static_cast<size_t>(gbid.row) + 1;
    size_t const cols =
        (gbid.col < _gbb_mat.cols()) ?
            _gbb_mat.cols() : static_cast<size_t>(gbid.col) + 1;
    _gbb_mat.reshape(rows, cols);
    return _gbb_mat.at(gbid.row, gbid.col);
}

grid_dim_t mm_gbb_buffer::dim() const {
    return grid_dim_t{  
        static_cast<uint32_t>(_gbb_mat.cols()), 
        static_cast<uint32_t>(_gbb_mat.rows()) 
    };
}

void mm_gbb_buffer::clear() {
    _gbb_mat.reset();
}

} // !namespace _generator
} // !namespace grid_format
} // !namespace gstream