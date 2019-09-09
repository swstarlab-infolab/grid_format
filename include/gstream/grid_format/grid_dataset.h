#ifndef _GSTREAM_GRID_FORMAT_GRID_H_
#define _GSTREAM_GRID_FORMAT_GRID_H_
#include <gstream/grid_format/grid_block.h>
#include <mixx/detail/noncopyable.h>
#include <string>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

namespace gstream {
namespace grid_format {

struct grid_dataset_info {
    void printout() const;

    grid_dim_t dim;
    uint64_t page_size;
    uint64_t seg_dist[21];
    // edge info
    uint64_t edge_count;
    struct {
        uint64_t sparse_edge_count;
        uint64_t dense_edge_count;
        //uint64_t long_edge_count;
    } edge_prop;
    // segment info
    uint64_t seg_count;
    struct {
        uint64_t sparse_seg_count;
        uint64_t dense_seg_count;
        //uint64_t long_seg_count;
    } seg_prop;
    // row info
    uint64_t row_count;
    /* struct {
        uint64_t lrow_count; // long row count
        uint64_t srow_count; // segmented row count
        uint64_t row_ind_count;  // row indicator count
    } row_prop; */
    uint64_t in_memory_size;
    double mean_edge_per_seg;
    //double mean_edge_per_seg_wo_long;
    double mean_edge_per_gb;
    double mean_edge_per_row;
    double mean_in_memory_gb_size;
    //double mean_folded_gb_size;
};

struct gb_idx_tuple {
    uint64_t offset;
    uint32_t part;
    uint32_t ptr;
    MIXX_FORCEINLINE void printout() const {
        printf("%14" PRIu64 "| %4u| %14u |", offset, part, ptr);
    }
};

struct rgb_idx_tuple {
    uint64_t offset;
    uint64_t size;
};

std::string make_metadata_path(char const* name, char const* dir);
std::string make_grid_path(char const* name, char const* dir);

class grid_index_table final {
public:
    grid_index_table();
    ~grid_index_table() = default;
    explicit grid_index_table(grid_dim_t const& dim, gb_idx_tuple* base_addr);
    void init(grid_dim_t const& dim, gb_idx_tuple* base_addr);
    gb_idx_tuple& operator[](gbid_t const& gbid) const;

    gb_idx_tuple* base_addr() const {
        return _entries;
    }

    void printout() const;

private:
    grid_dim_t _dim;
    gb_idx_tuple* _entries;
};

class mm_grid_dataset: mixx::noncopyable {
public:
    mm_grid_dataset();
    ~mm_grid_dataset();
    bool import_from_file(char const* name, char const* dir);

    grid_dataset_info const& info() const {
        return _info;
    }

protected:
    grid_dataset_info _info;
    grid_index_table  _idxtbl;
};

size_t seg_hash(size_t const seg_length);

} // !namespace grid_format
} // !namespace gstream

#endif // !_GSTREAM_GRID_FORMAT_GRID_H_
