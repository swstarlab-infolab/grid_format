#ifndef _GSTREAM_GRID_FORMAT_H_
#define _GSTREAM_GRID_FORMAT_H_
#include <gstream/grid_format/virtual_grid.h>

namespace gstream {

using _grid_format::encoded_vid;
using _grid_format::encoded_edge;
using _grid_format::global_edge;
using _grid_format::pid_t;
using _grid_format::cbid_t;
using _grid_format::l0_size_t;
using _grid_format::l1_size_t;
using _grid_format::l2_size_t;

using _grid_format::gbid_t;
using _grid_format::cb_physical_pointer;
using _grid_format::cb_logical_pointer;

using _grid_format::cell_block;
using _grid_format::grid_page;
using _grid_format::sparse_grid_block;
using _grid_format::cell_block_column;

using _grid_format::grid_dim_t;

using _grid_format::virtual_grid_block;

GSTREAM_GRID_FORMAT_TEMPLATE_W_DEFAULT_PARAMS
struct grid_format_template {
    GSTREAM_ALIAS_GRID_FORMAT_TEMPLATE_ARGS;
    static_assert(_grid_format::max_grid_block_width<page_size>::value >= gb_width, "page size is too small!");
    using page_type = grid_page<page_size, alignment>;
    using sparse_gb = sparse_grid_block<grid_format_template>;
    using gb_iterator = typename sparse_gb::iterator;
    using gb_column = cell_block_column;
    using column_iterator = gb_column::iterator;
};

using default_grid_format = grid_format_template<65536, mixx::MiB(4)>;

template <typename GridFormat>
void restore_edge(global_edge* e_out, gbid_t const& gbid, encoded_edge const& encoded) {
    e_out->src = encoded.src + (gbid.row * GridFormat::gb_width);
    e_out->dst = encoded.dst + (gbid.col * GridFormat::gb_width);
}

template <typename GridFormat>
global_edge restore_edge(gbid_t const& gbid, encoded_edge const& encoded) {
    global_edge e;
    e.src = encoded.src + (gbid.row * GridFormat::gb_width);
    e.dst = encoded.dst + (gbid.col * GridFormat::gb_width);
    return e;
}

} // !namespace gstream

#endif // !_GSTREAM_GRID_FORMAT_H_
