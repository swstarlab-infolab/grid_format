#ifndef _GSTREAM_GRID_FORMAT_GRID_DATASET_H_
#define _GSTREAM_GRID_FORMAT_GRID_DATASET_H_
#include <gstream/error.h>
#include <gstream/grid_format.h>
#include <gstream/grid_format/grid_dataset_info.h>

namespace gstream {

using _grid_format::gbi_node;
using _grid_format::gb_direct_pointer_block;
using _grid_format::gb_indirect_pointer_block;
using _grid_format::gb_direct_pointer_block_physical;
using _grid_format::gb_indirect_pointer_block_physical;

class in_memory_device_grid_dataset;

class in_memory_grid_dataset final: mixx::noncopyable {
    friend class in_memory_device_grid_dataset;
public:
    in_memory_grid_dataset();
    ~in_memory_grid_dataset() noexcept;
    void reset();
    gstream_error_code import(char const* name, char const* dir); 

    gbi_node* gbi(gbid_t const& gbid) const {
        return _gbi_index[gbid.col * info.dim.y + gbid.row];
    }

    bool is_empty_gb(gbid_t const& gbid) const {
        return gbi(gbid) == nullptr;
    }

    grid_dataset_info info;
private:
    void _cleanup_buffer();
    gbi_node** _gbi_index;
    struct {
       char* topology;
        gbi_node* gbi;
        gb_indirect_pointer_block_physical* gipb_p;
        gb_indirect_pointer_block* gipb;
    } _buffer;
};

class in_memory_device_grid_dataset final: mixx::noncopyable {
public:
    in_memory_device_grid_dataset();
    ~in_memory_device_grid_dataset() noexcept;
    gstream_error_code import(in_memory_grid_dataset const& source);
    void reset();

    MIXX_FORCEINLINE gbi_node* gbi(gbid_t const& gbid) const {
        return _gbi_index[gbid.col * dim.y + gbid.row];
    }

    MIXX_FORCEINLINE bool is_emptry_gb(gbid_t const& gbid) const {
        return gbi(gbid) == nullptr;
    }

    grid_dim_t dim;
private:
    gstream_error_code _cleanup_buffer() noexcept;
    gbi_node** _gbi_index;
    struct {
        char* topology_dev;
        gbi_node* gbi_dev;
        gb_indirect_pointer_block* gipb_dev;
    } _devbuf;
};

} // !namespace gstream

#endif // !_GSTREAM_GRID_FORMAT_GRID_DATASET_H_
