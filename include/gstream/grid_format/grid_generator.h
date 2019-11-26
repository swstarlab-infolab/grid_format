/*!
 * GStream project by InfoLab @ DGIST (https://infolab.dgist.ac.kr)
 * Copyright 2019 GStream Authors. All Rights Reserved.
 * 
 * <License>
 * 
 * @file gstream/grid_format/grid_generator.h
 * @project LibGStream
 * @brief Grid dataset generator (converter)
 * @author Seyeon Oh (mailto:vee@dgist.ac.kr)
 * @version 1.0 4/25/2019
 */
#ifndef _GSTREAM_GRID_FORMAT_GRID_GENERATOR_H_
#define _GSTREAM_GRID_FORMAT_GRID_GENERATOR_H_

#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#define _UNDEF_CRT_SECURE_NO_DEPRECATE
#endif // !_CRT_SECURE_NO_DEPRECATE

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#define _UNDEF_CRT_SECURE_NO_WARNINGS
#endif // !_CRT_SECURE_NO_WARNINGS

#include <gstream/grid_format/gbi.h>
#include <gstream/grid_format/grid_page.h>
#include <gstream/grid_format/grid_dataset_info.h>
#include <gstream/error.h>
#include <mixx/property.h>
#include <mixx/pool.h>
#include <assert.h>
#include <vector>
#include <map>
#include <algorithm>

namespace gstream {

namespace _grid_format {

/*============================
 * Grid block resource block
 * ==========================*/

template <typename GridFormat>
class gb_resource_container: mixx::noncopyable {
public:
    GSTREAM_ALIAS_GRID_FORMAT_INTERNAL_DEFINES(GridFormat);
    gb_resource_container();
    explicit gb_resource_container(gbid_t);
    ~gb_resource_container() noexcept = default;

    void add_edge(const global_edge& e);
    void sort();
    void clear_data();
    void reset(gbid_t);
    gbid_t get_id() const;

    std::vector<encoded_edge> encoded_edges;  
    gb_edge_bitmap row_bitmap;
    gb_edge_bitmap col_bitmap;
    l1_size_t column_size[gb_width];
    
protected:
    gbid_t _gbid;
    l0_size_t _global_col_off;
    l0_size_t _global_row_off;
};

template <typename GridFormat>
gb_resource_container<GridFormat>::gb_resource_container() {
    reset(gbid_t{ 0, 0});
}

template <typename GridFormat>
gb_resource_container<GridFormat>::gb_resource_container(gbid_t gbid) {
    reset(gbid);
}

template <typename GridFormat>
void gb_resource_container<GridFormat>::add_edge(const global_edge& e) {
    assert(/* boundary checking */ e.src >= _global_row_off && e.src < (_global_row_off + gb_width));
    assert(/* boundary checking */e.dst >= _global_col_off && e.dst < (_global_col_off + gb_width));
    assert(/* overflow checking */ (e.src - _global_row_off) <= UINT16_MAX );
    assert(/* overflow checking */ (e.dst - _global_col_off) <= UINT16_MAX );
    encoded_edge encoded;
    encoded.src = static_cast<encoded_vid>(e.src - _global_row_off);
    encoded.dst = static_cast<encoded_vid>(e.dst - _global_col_off);
    if (!row_bitmap.test(encoded.src))
        row_bitmap.set(encoded.src);
    if (!col_bitmap.test(encoded.dst))
        col_bitmap.set(encoded.dst);
    column_size[encoded.dst] += 1;
    assert(column_size[encoded.dst] <= gb_width);
    encoded_edges.emplace_back(std::move(encoded));
}

template <typename GridFormat>
void gb_resource_container<GridFormat>::sort() {
    std::sort(encoded_edges.begin(), encoded_edges.end(), [](encoded_edge const& lhs, encoded_edge const& rhs) {
        if (lhs.dst < rhs.dst)
            return true;
        if (lhs.dst > rhs.dst)
            return false;
        return lhs.src < rhs.src;
    });
}

template <typename GridFormat>
void gb_resource_container<GridFormat>::clear_data() {
    encoded_edges.clear();
    row_bitmap.clear_all();
    col_bitmap.clear_all();
    memset(column_size, 0, sizeof(l1_size_t) * gb_width);
}

template <typename GridFormat>
void gb_resource_container<GridFormat>::reset(gbid_t gbid) {
    _gbid = gbid;
    _global_col_off = gbid.col * gb_width;
    _global_row_off = gbid.row * gb_width;
    clear_data();
}

template <typename GridFormat> MIXX_FORCEINLINE 
gbid_t gb_resource_container<GridFormat>::get_id() const {
    return _gbid;
}

/*============================
 * Cell block building block
 * ==========================*/

template <typename GridFormat>
class cb_buliding_block: mixx::noncopyable {
public:
    GSTREAM_ALIAS_GRID_FORMAT_INTERNAL_DEFINES(GridFormat);
    MIXX_USING_CXX_PROPERTY(cb_buliding_block);
    cb_buliding_block();
    ~cb_buliding_block() noexcept = default;
    l1_size_t scan() const;
    void put_column(l1_size_t length);
    void reset(l1_size_t phys_size);
    l1_size_t size() const;

    MIXX_FORCEINLINE bool is_empty() const {
        return column_count == 0u;
    }

    protected_property<l1_size_t> free;
    protected_property<l1_size_t> size_rows;
    protected_property<l1_size_t> size_ptrs;
    protected_property<l1_size_t> size_cols;
    protected_property<l1_size_t> edge_count;
    protected_property<l1_size_t> column_count;
};

template <typename GridFormat>
cb_buliding_block<GridFormat>::cb_buliding_block() {
    free = 0u;
    size_rows = 0u;
    size_ptrs = 0u;
    size_cols = 0u;
    edge_count = 0u;
    column_count = static_cast<l1_size_t>(0);
}

template <typename GridFormat>
l1_size_t cb_buliding_block<GridFormat>::scan() const {
    if (free < cell_block::minimum_column_data_size)
        return 0;
    l1_size_t avail = free;
    avail -= sizeof(cell_block::colptr_t); // colptr for a new column
    avail -= sizeof(encoded_vid); // colidx for the new column
    avail /= sizeof(encoded_vid);
    return avail > gb_width ? gb_width : avail;
}

template <typename GridFormat>
void cb_buliding_block<GridFormat>::put_column(l1_size_t length) {
    assert( length > 0 );
    l1_size_t const rows_data_size = sizeof(encoded_vid) * length;
    size_rows += rows_data_size;
    size_ptrs += static_cast<l1_size_t>(sizeof(cell_block::colptr_t));
    size_cols += static_cast<l1_size_t>(sizeof(encoded_vid));
    free -= rows_data_size + static_cast<l1_size_t>(sizeof(cell_block::colptr_t) + sizeof(encoded_vid));
    edge_count += length;
    column_count += static_cast<l1_size_t>(1);
    assert( column_count.get() <= gb_width );
}

template <typename GridFormat>
void cb_buliding_block<GridFormat>::reset(l1_size_t phys_size) {
    assert( phys_size % alignment == 0 );
    if (phys_size >= cell_block::minimum_size) {
        free = phys_size - cell_block::dummy_size - static_cast<l1_size_t>(sizeof(cell_block_header));
        size_rows = 0;
        size_ptrs = static_cast<l1_size_t>(sizeof(cell_block::colptr_t)); // dummy pointer
        size_cols = 0;
        edge_count = 0;
        column_count = static_cast<l1_size_t>(0);
    }
    else {
        free = 0u;
        size_rows = 0u;
        size_ptrs = 0u;
        size_cols = 0u;
        edge_count = 0u;
        column_count = static_cast<l1_size_t>(0); 
    }
    
}

template <typename GridFormat>
l1_size_t cb_buliding_block<GridFormat>::size() const {
    return sizeof(cell_block_header) + size_rows + size_ptrs + size_cols;
}

/*============================
 * Page building block
 * ==========================*/

template <typename GridFormat>
class page_building_block: mixx::noncopyable {
public:
    GSTREAM_ALIAS_GRID_FORMAT_INTERNAL_DEFINES(GridFormat);
    MIXX_USING_CXX_PROPERTY(page_building_block);
    using page_type = grid_page<page_size, alignment>;

    page_building_block();
    ~page_building_block() noexcept = default;
    l1_size_t scan();
    void reset();
    l1_size_t next_cb_offset() const;
    void put_cell_block(l1_size_t cb_size);

    protected_property<l1_size_t> free;
    protected_property<l1_size_t> size_cbs;
    protected_property<l1_size_t> size_ptrs;
    protected_property<l1_size_t> cb_count;
    protected_property<l1_size_t> padding_count;
    std::vector<l1_size_t> index_vec;
};

template <typename GridFormat>
page_building_block<GridFormat>::page_building_block() {
    reset();
}

template <typename GridFormat>
l1_size_t page_building_block<GridFormat>::scan() {
    if (free < cell_block::minimum_size + sizeof(l1_size_t))
        return 0;
    return free - static_cast<l1_size_t>(sizeof(l1_size_t));
}

template <typename GridFormat>
void page_building_block<GridFormat>::reset() {
    free = page_type::data_section_size;
    size_cbs = 0;
    size_ptrs = 0;
    cb_count = 0;
    padding_count = 0;
    index_vec.clear();
}

template <typename GridFormat>
l1_size_t page_building_block<GridFormat>::next_cb_offset() const {
    return sizeof(grid_page_header) + size_cbs.get() + padding_count.get();
}

template <typename GridFormat>
void page_building_block<GridFormat>::put_cell_block(l1_size_t cb_size) {
    assert(cb_size < page_size);
    assert( free >= cb_size + sizeof(l1_size_t) );
    index_vec.emplace_back(next_cb_offset());
    l1_size_t padding = mixx::padding_size(cb_size, alignment);
    free -= cb_size + static_cast<l1_size_t>(sizeof(l1_size_t)) + padding;
    size_cbs += cb_size;
    size_ptrs += static_cast<l1_size_t>(sizeof(l1_size_t));
    cb_count += 1;
    padding_count += padding;
}

/*============================
 * Grid resource builder
 * ==========================*/
template <typename GridFormat>
class grid_resource_builder: mixx::noncopyable {
public:
    GSTREAM_ALIAS_GRID_FORMAT_INTERNAL_DEFINES(GridFormat);
    MIXX_USING_CXX_PROPERTY(grid_resource_builder);
    using gbr_type = gb_resource_container<grid_format>;
    using gbr_map_t = std::map<gbid_t, gbr_type*>;

    grid_resource_builder();
    ~grid_resource_builder() noexcept;
    void add_edge(global_edge const& e);
    void reset();

    MIXX_FORCEINLINE gbr_map_t& gbr_map() {
        return _gbr_map;
    }

    protected_property<l1_size_t> max_col;
    protected_property<l1_size_t> max_row;
protected:
    gbr_map_t _gbr_map;
};

/*============================
 * Grid resource builder
 * ==========================*/

template <typename GridFormat>
grid_resource_builder<GridFormat>::grid_resource_builder() {
    max_col = 0;
    max_row = 0;
}

template <typename GridFormat>
grid_resource_builder<GridFormat>::~grid_resource_builder() noexcept {
    reset();
}

template <typename GridFormat>
void grid_resource_builder<GridFormat>::add_edge(global_edge const& e) {
    gbid_t gbid = edge_to_gbid<grid_format>(e);
    auto kv = _gbr_map.find(gbid);
    if (kv != std::end(_gbr_map)) {
        gbr_type*& gbr = kv->second;
        gbr->add_edge(e);
    }
    else {
        gbr_type* gbr = new gbr_type(gbid);
        _gbr_map.insert(std::make_pair(gbid, gbr));
        if (max_col < gbid.col)
            max_col = gbid.col;
        if (max_row < gbid.row)
            max_row = gbid.row;
        gbr->add_edge(e);
    }
}

template <typename GridFormat>
void grid_resource_builder<GridFormat>::reset() {
    for (auto& kv : _gbr_map) {
        gbr_type*& gbr = kv.second;
        delete gbr;
    }
    _gbr_map.clear();
}

/*============================
 * Grid generator
 * ==========================*/

template <typename GridFormat>
class grid_generator final: mixx::noncopyable {
public:
    GSTREAM_ALIAS_GRID_FORMAT_INTERNAL_DEFINES(GridFormat);
    using gbr_type = gb_resource_container<grid_format>;
    using cbb_type = cb_buliding_block<grid_format>;
    using pbb_type = page_building_block<grid_format>;
    using grb_type = grid_resource_builder<grid_format>;
    using page_type = grid_page<page_size, alignment>;

    grid_generator();
    ~grid_generator() noexcept;

    void add_edge(global_edge const& e);
    gstream_error_code generate(char const* dataset_name, char const* outdir);
    gstream_error_code write_file(grid_dataset_info const& info, char const* outdir);
    void reset_buffer();
    void reset();

private:
    void _build_cb(gbr_type const& gbr, cbid_t cbid, l2_size_t col_begin, l2_size_t col_end, l1_size_t e_off);
    void _flush_cb();
    void _build_page();
    void _flush_page(); 
    void _build_gb(gbr_type& gbr);

    /*============================
     * Local class: output buffer
     * ==========================*/

    class output_buffer {
    public: 
        output_buffer();
        ~output_buffer() noexcept;
        void reset();
        MIXX_FORCEINLINE page_type* add_page();
        MIXX_FORCEINLINE gbi_node*  add_gbi();
        MIXX_FORCEINLINE gb_indirect_pointer_block_physical* add_gipb();
        MIXX_FORCEINLINE page_type* last_page_pointer();
        MIXX_FORCEINLINE pid_t      last_page_id();
        MIXX_FORCEINLINE gbi_node*  last_gbi_pointer();
        MIXX_FORCEINLINE gb_indirect_pointer_block_physical* last_gipb();
        MIXX_FORCEINLINE physical_pointer last_gipb_id() const;
        MIXX_FORCEINLINE pid_t page_count();
        MIXX_FORCEINLINE gb_edge_bitmap* get_grid_row_bitmap();
        MIXX_FORCEINLINE gb_edge_bitmap* get_grid_col_bitmap();
        template <typename T> MIXX_FORCEINLINE l0_size_t get_max_outdegree() const;
        template <typename T> MIXX_FORCEINLINE T get_outdegree_subvector(l1_size_t gb_row);

        void init_bitmap(grid_dim_t const& grid_dim);
        gstream_error_code init_outdegree_vector(l0_size_t length);

        gstream_error_code flush_page(FILE* fp);
        gstream_error_code flush_gbi(FILE* fp);
        gstream_error_code flush_gipb(FILE* fp);
        gstream_error_code flush_outdegree(FILE* fp) const;
    protected:
        void _cleanup();
        mixx::object_pool<page_type, 128> _page_pool;
        mixx::object_pool<gbi_node, 1024> _gbi_pool;
        mixx::object_pool<gb_indirect_pointer_block_physical, 512> _gipb_pool;
        std::vector<page_type*> _page_vec;
        std::vector<gbi_node*> _gbi_vec;
        std::vector<gb_indirect_pointer_block_physical*> _gipb_vec; 
        gb_edge_bitmap* _grid_row_bitmap { nullptr };
        gb_edge_bitmap* _grid_col_bitmap { nullptr };
        void* _outdegree { nullptr };
        l0_size_t _outdegree_size { 0 };
        l0_size_t _outdegree_length { 0 };
    } outbuf;

    l0_size_t max_vid;
    l0_size_t max_outdegree;
    grb_type grb;
    cbb_type cbb;
    pbb_type pbb;
};

template <typename GridFormat>
grid_generator<GridFormat>::grid_generator() {
    reset();
}

template <typename GridFormat>
grid_generator<GridFormat>::~grid_generator() noexcept {
}

template <typename GridFormat>
void grid_generator<GridFormat>::add_edge(global_edge const& e) {
    grb.add_edge(e);
    l0_size_t const bigger = e.src > e.dst ? e.src : e.dst;
    if (max_vid < bigger)
        max_vid = bigger;
}

template <typename GridFormat>
gstream_error_code grid_generator<GridFormat>::generate(char const* dataset_name, char const* outdir) {
    // reset existing status and outputs
    reset_buffer();

    // intialize dataset information (zero-fill) 
    grid_dataset_info info;
    memset(&info, 0, sizeof(grid_dataset_info));
    auto& gbr_map = grb.gbr_map();

    // set fixed values of dataset information
    snprintf(info.name, GSTREAM_DATASET_NAME_MAX, dataset_name);
    snprintf(info.toolchain_name, GSTREAM_DATASET_NAME_MAX, MIXX_TOOLCHAIN_NAME);
    info.gb_width = gb_width;
    info.page_size = page_size;
    info.dim.x = info.dim.y = std::max(grb.max_col.get() + 1, grb.max_row.get() + 1);
    info.gbi_count = gbr_map.size();
    info.blank_count = info.dim.size() - info.gbi_count;
    info.max_vid = max_vid;

    // initialize output bitmap buffer and outdegree buffer
    outbuf.init_bitmap(info.dim);
    outbuf.init_outdegree_vector(max_vid + 1);
    
    // setup references to make clear code
    gb_edge_bitmap* grid_row_bitmap = outbuf.get_grid_row_bitmap();
    gb_edge_bitmap* grid_col_bitmap = outbuf.get_grid_col_bitmap();

    // iterate the Grid Resource Builder (grid_resource_builder, GRB); <Main Loop>
    for (auto& kv : gbr_map) {
        gbid_t gbid = kv.first; // key
        gbr_type* gbr = kv.second; // Grid Block Resource container (gb_resource_container, GBR)
        _build_gb(*gbr);
        gbi_node* gbi = outbuf.last_gbi_pointer();
        //printf("gen> G[%u][%u], %u columns, %u edges\n", gbid.row, gbid.col, gbi->column_count, gbi->edge_count);
        // Update dataset information
        if (gbi->ipb_count == 0)
            info.sgbi_count += 1; // small gbi
        else
            info.gipb_count += gbi->ipb_count;
        info.cb_count += gbi->cb_count;
        if (gbi->page_count * page_size < gb_bitmap_size)
            info.sparse_count += 1;
        else
            info.dense_count += 1;
        info.edge_count += gbr->encoded_edges.size();
        grid_col_bitmap[gbid.col] |= gbr->col_bitmap;
        grid_row_bitmap[gbid.row] |= gbr->row_bitmap;
    }

    // flush last page
    if (pbb.cb_count.get() > 0) {
        _build_page();
        _flush_page(); // reset pbb and cbb
    }

    info.page_count = outbuf.last_page_id() + 1;
    if (max_vid >= UINT32_MAX)
        info.max_outdegree = outbuf.template get_max_outdegree<l0_size_t>();
    else if (max_vid >= UINT16_MAX)
        info.max_outdegree = outbuf.template get_max_outdegree<l1_size_t>();
    else
        info.max_outdegree = outbuf.template get_max_outdegree<l2_size_t>();

    // calulate total # of vertices
    {
        gb_edge_bitmap merged;
        for (l1_size_t i = 0; i < info.dim.x; ++i) {
            merged.clear_all();
            merged = grid_row_bitmap[i] | grid_col_bitmap[i];
            info.vertex_count += merged.count();
        }
    }   

    printf("gridgen> dataset info\n----------------------------------------\n");
    info.printout();
    printf("----------------------------------------\n");

    return write_file(info, outdir);
}

template <typename GridFormat>
gstream_error_code grid_generator<GridFormat>::write_file(grid_dataset_info const& info, char const* outdir) {
    auto const create_file = [&](char const* ext) -> FILE* {
        char path[FILENAME_MAX];
        snprintf(path, FILENAME_MAX, "%s%c%s.%s", outdir, MIXX_PATH_SEPARATOR, info.name, ext);
        printf("gridgen> create %s\n", path);
        return fopen(path, "wb");
    };

    gstream_error_code result;
    FILE* md_file;
    FILE* page_file = nullptr;
    FILE* outdegree_file = nullptr;

    // 1. make a metadata
    md_file = create_file("grid_md");
    if (md_file == nullptr)
        return GStreamCreateFileFailure;

    // 1.1. dataset information
    {
        size_t const r = fwrite(&info, sizeof(grid_dataset_info), 1, md_file);
        if (r != 1) {
            result = GStreamFileWriteFailure;
            goto lb_return;
        }
    }

    // 1.2. gbi node
    {
        result = outbuf.flush_gbi(md_file);
        if (result != GStreamSuccess)
            goto lb_return;
    }

    // 1.3. indirect pointer block
    {
        result = outbuf.flush_gipb(md_file);
        if (result != GStreamSuccess)
            goto lb_return;
    }

    // 2. make a page file
    page_file = create_file("grid_pages");
    if (page_file == nullptr) {
        result = GStreamCreateFileFailure;
        goto lb_return; 
    }
    else {
        result = outbuf.flush_page(page_file);
        if (result != GStreamSuccess)
            goto lb_return;
    }

    // 3. make an outdegree file
    outdegree_file = create_file("outdegree");
    if (outdegree_file == nullptr) {
        result = GStreamCreateFileFailure;
        goto lb_return;
    }
    else {
        result = outbuf.flush_outdegree(outdegree_file);
        if (result != GStreamSuccess)
            goto lb_return;
    }

lb_return:
    if (md_file != nullptr)
        fclose(md_file);
    if (page_file != nullptr)
        fclose(page_file);
    if (outdegree_file != nullptr)
        fclose(outdegree_file);
    return result;
}

template <typename GridFormat>
void grid_generator<GridFormat>::reset_buffer() {
    pbb.reset();
    cbb.reset(pbb.scan());
    outbuf.reset();
}

template <typename GridFormat>
void grid_generator<GridFormat>::reset() {
    grb.reset();
    reset_buffer();
    max_vid = 0;
    max_outdegree = 0;
}

template <typename GridFormat>
void grid_generator<GridFormat>::_build_cb(gbr_type const& gbr, cbid_t cbid, l2_size_t const col_begin, l2_size_t const col_end, l1_size_t e_off) {
    // get a physical offset for a next cell block
    l1_size_t const phys_off = pbb.next_cb_offset();
    // get a target buffer address
    void* buffer;
    {
        // get a target page
        page_type* page = outbuf.last_page_pointer();
        // seek a pointer to next free space
        buffer = mixx::seek_pointer(page, phys_off);
    }
    
    // make a cell block header
    {
        cell_block_header* header = static_cast<cell_block_header*>(buffer);
        header->gbid = gbr.get_id();
        header->cbid = cbid;
        header->pid  = outbuf.page_count() - 1;
        header->offset.physical = phys_off;
        header->offset.ptrs = 0;
        header->offset.rows = cbb.size_ptrs.get();
        header->offset.cols = cbb.size_ptrs.get() + cbb.size_rows.get();
        header->column_count = cbb.column_count.get();
        header->edge_count = cbb.edge_count.get();
        assert(mixx::is_aligned_offset(header->offset.ptrs, sizeof(cell_block::colptr_t)));
        assert(mixx::is_aligned_offset(header->offset.rows, sizeof(encoded_vid)));
        assert(mixx::is_aligned_offset(header->offset.cols, sizeof(encoded_vid)));
    }

    // get a relative outdegree buffer and aggregate funcptr
    void* outdegree;
    void (*aggregate_outdegree)(void*, l2_size_t);
    if (max_vid >= UINT32_MAX) {
         outdegree = outbuf.template get_outdegree_subvector<l0_size_t*>(gbr.get_id().row);
         aggregate_outdegree = [](void* outdegree, l2_size_t const src) {
             static_cast<l0_size_t*>(outdegree)[src] += 1;
         };
    }
    else if (max_vid >= UINT16_MAX) {
        outdegree= outbuf.template get_outdegree_subvector<l1_size_t*>(gbr.get_id().row);
        aggregate_outdegree = [](void* outdegree, l2_size_t const src) {
             static_cast<l1_size_t*>(outdegree)[src] += 1;
         };
    }
    else {
        outdegree= outbuf.template get_outdegree_subvector<l2_size_t*>(gbr.get_id().row);
        aggregate_outdegree = [](void* outdegree, l2_size_t const src) {
             static_cast<l2_size_t*>(outdegree)[src] += 1;
         };
    }
   
    // make a cell block data
    {
        cell_block* cb = static_cast<cell_block*>(buffer);
        cell_block::colptr_t* ptrs = cb->ptrs();
        encoded_vid* rows = cb->rows();
        encoded_vid* cols = cb->cols();
        l1_size_t curr_rows_off = cb->offset.rows;
        l1_size_t rows_inserted = 0;
        l1_size_t cols_inserted = 0;
        //printf("debug> gbr.edges = %u, cb->edge_count = %u\n", gbr.encoded_edges.size(), cb->edge_count);
        for (l1_size_t i = col_begin; i <= col_end; ++i) {
            if (gbr.col_bitmap.test(i) == false)
                continue; // empty column
            l1_size_t const column_size = gbr.column_size[i];
            // set a column pointer
            ptrs[cols_inserted] = rows_inserted;
            // update a current rows offset
            curr_rows_off += column_size * sizeof(encoded_vid);
            // set values into the rows
            //printf("debug> G[%u][%u], cur_col(i): %u, gbr.edges: %llu, rows_inserted: %u, col_size: %u, col_begin: %u, col_end: %u\n", 
            //       gbr.get_id().col, gbr.get_id().row, i, gbr.encoded_edges.size(), rows_inserted, column_size, col_begin, col_end);
            for (l1_size_t j = 0; j < column_size; ++j, ++rows_inserted) {
                l2_size_t const src = gbr.encoded_edges[e_off + rows_inserted].src;
                rows[rows_inserted] = src;
                aggregate_outdegree(outdegree, src);
            }
            // set a column index
            cols[cols_inserted] = static_cast<l2_size_t>(i);
            cols_inserted += 1;
        }
        assert(rows_inserted == cb->edge_count);
        //set a dummy pointer
        ptrs[cb->column_count] = cb->edge_count;
    }
}

template <typename GridFormat>
void grid_generator<GridFormat>::_flush_cb() {
    pbb.put_cell_block(cbb.size());
    cbb.reset(pbb.scan());
}

template <typename GridFormat>
void grid_generator<GridFormat>::_build_page() {
    page_type* page = outbuf.last_page_pointer();
    // make a page header
    {
        page->pid = outbuf.last_page_id();
        page->cb_count = pbb.cb_count.get();
        page->free = pbb.free.get();
    }
    // make an index
    {
        l1_size_t* index = page->index();
        assert(pbb.index_vec.size() == pbb.cb_count.get());
        for (l1_size_t i = 0; i < pbb.cb_count.get(); ++i)
            index[i] = pbb.index_vec[pbb.cb_count.get() - (i + 1)];
    }
}

template <typename GridFormat>
void grid_generator<GridFormat>::_flush_page() {
    outbuf.add_page();
    pbb.reset();
    cbb.reset(pbb.scan());
}

template <typename GridFormat>
void grid_generator<GridFormat>::_build_gb(gbr_type& gbr) {

    auto concatenate_cb_index = [this](gbi_node* gbi, cbid_t cbid, cb_physical_pointer ptr, l2_size_t agg_col) {
        if (cbid < DirectBlockSize) {
            l2_size_t const ptr_idx = cbid;
            gbi->direct_p.ptr[ptr_idx] = ptr;
            gbi->direct_p.col_count[ptr_idx] = cbb.column_count.get();
            gbi->direct_p.agg_col[ptr_idx] = agg_col;
            gbi->direct_p.size += 1;
        }
        else if (cbid == DirectBlockSize) {
            gb_indirect_pointer_block_physical* gipb = outbuf.add_gipb();
            gbi->indirect_p = outbuf.last_gipb_id();
            gbi->ipb_count += 1;
            gipb->size = 1;
            gipb->ptr[0] = ptr;
            gipb->col_count[0] = cbb.column_count.get();
            gipb->agg_col[0] = agg_col;
        }
        else {
            l2_size_t const ptr_idx = (cbid - DirectBlockSize) % IndirectBlockSize;
            gb_indirect_pointer_block_physical* gipb;
            if (ptr_idx == 0) {
                gb_indirect_pointer_block_physical* prev = outbuf.last_gipb();
                gipb = outbuf.add_gipb();
                prev->next = outbuf.last_gipb_id();
                gbi->ipb_count += 1;
            }
            else {
                gipb = outbuf.last_gipb();
            }
            gipb->ptr[ptr_idx] = ptr;
            gipb->col_count[ptr_idx] = cbb.column_count.get();
            gipb->agg_col[ptr_idx] = agg_col;
            gipb->size += 1;
        }
    };

    assert(gbr.encoded_edges.size() <= std::numeric_limits<l1_size_t>::max());
    gbr.sort();

    // add a new gbi-node
    gbi_node* gbi = outbuf.add_gbi();
    // initialize the gbi-node
    memset(gbi, 0, sizeof(gbi_node));
    gbi->id = gbr.get_id();

    // local variables to represent generation status
    l2_size_t col_begin = 0;
    l1_size_t e_off = 0; // edge offset
    l1_size_t agg_column_count = 0;
    for (l1_size_t i = 0; i < gb_width; ++i) {
        if (gbr.col_bitmap.test(i) == false)
            continue; // empty column
        
        /* Scan the cbb to determine if there is enough space 
           in the cell block to store the current column */
        l1_size_t const column_size = gbr.column_size[i];
        gbi->column_count += 1;
        gbi->edge_count += column_size;

        if (cbb.scan() < column_size) {
            // cbb does not have enough space
            if (!cbb.is_empty()) {
                cbid_t const cbid = gbi->cb_count;
                // Current cell block is not empty, make and flush it
                assert(i != 0);
                _build_cb(gbr, cbid, col_begin, static_cast<l2_size_t>(i - 1), e_off);
                e_off += cbb.edge_count.get(); // update the next edge offset
                agg_column_count += cbb.column_count.get();
                concatenate_cb_index(gbi, cbid, cb_physical_pointer{ outbuf.last_page_id(), pbb.cb_count.get() }, static_cast<l2_size_t>(agg_column_count - 1));
                _flush_cb(); // reset cbb
                col_begin = static_cast<l2_size_t>(i); // update the next begin
                gbi->cb_count += 1;
                gbi->page_count += 1;
            }
            _build_page();
            _flush_page(); // reset pbb and cbb
        }
        assert(column_size <= gb_width);
        cbb.put_column(column_size);
    }

    // Process a last cell block
    if (!cbb.is_empty()) {
        cbid_t const cbid = gbi->cb_count;
        _build_cb(gbr, cbid, col_begin, gb_width - 1, e_off);
        agg_column_count += cbb.column_count.get();
        concatenate_cb_index(gbi, cbid, cb_physical_pointer{ outbuf.last_page_id(), pbb.cb_count.get() }, static_cast<l2_size_t>(agg_column_count - 1));
        _flush_cb(); // reset cbb
        gbi->cb_count += 1;
        gbi->page_count += 1;
    }
    // Note that we do not flush the page at this phase

    gbi->flags |= GSTREAM_SPARSE_GRID;
    //TODO: dense grid block
}

template <typename GridFormat>
grid_generator<GridFormat>::output_buffer::output_buffer() {
    reset();
}

template <typename GridFormat>
grid_generator<GridFormat>::output_buffer::~output_buffer() noexcept {
    _cleanup();
}

template <typename GridFormat> MIXX_FORCEINLINE
void grid_generator<GridFormat>::output_buffer::reset() {
    _cleanup();
    add_page();
}

template <typename GridFormat> MIXX_FORCEINLINE
typename grid_generator<GridFormat>::page_type* 
grid_generator<GridFormat>::output_buffer::add_page() {
    page_type* page = _page_pool.allocate();
    memset(page, 0, page_size);
    _page_vec.push_back(page);
    return page;
}

template <typename GridFormat> MIXX_FORCEINLINE
gbi_node* grid_generator<GridFormat>::output_buffer::add_gbi() {
    gbi_node* gbi = _gbi_pool.allocate();
    memset(gbi, 0, sizeof(gbi_node));
    set_invalid(gbi->indirect_p);
    _gbi_vec.push_back(gbi);
    return gbi;
}

template <typename GridFormat> MIXX_FORCEINLINE
gb_indirect_pointer_block_physical* grid_generator<GridFormat>::output_buffer::add_gipb() {
    gb_indirect_pointer_block_physical* gipb = _gipb_pool.allocate();
    memset(gipb, 0, sizeof(gb_indirect_pointer_block_physical));
    set_invalid(gipb->next);
    _gipb_vec.push_back(gipb);
    return gipb;
}

template <typename GridFormat> MIXX_FORCEINLINE 
typename grid_generator<GridFormat>::page_type*
grid_generator<GridFormat>::output_buffer::last_page_pointer() {
    return _page_vec.back();
}

template <typename GridFormat> MIXX_FORCEINLINE
pid_t grid_generator<GridFormat>::output_buffer::last_page_id() {
    return static_cast<pid_t>(_page_vec.size()) - 1;
}

template <typename GridFormat> MIXX_FORCEINLINE
gbi_node* grid_generator<GridFormat>::output_buffer::last_gbi_pointer() {
    return _gbi_vec.back();
}

template <typename GridFormat> MIXX_FORCEINLINE
gb_indirect_pointer_block_physical* grid_generator<GridFormat>::output_buffer::last_gipb() {
    return _gipb_vec.back();
}

template <typename GridFormat> MIXX_FORCEINLINE
physical_pointer grid_generator<GridFormat>::output_buffer::last_gipb_id() const {
    return physical_pointer{ 0, static_cast<uint32_t>(_gipb_vec.size() - 1)};
}

template <typename GridFormat> MIXX_FORCEINLINE
pid_t grid_generator<GridFormat>::output_buffer::page_count() {
    return static_cast<pid_t>(_page_vec.size());
}

template <typename GridFormat> MIXX_FORCEINLINE
typename grid_generator<GridFormat>::gb_edge_bitmap* grid_generator<GridFormat>::output_buffer::get_grid_row_bitmap() {
    return _grid_row_bitmap;
}

template <typename GridFormat> MIXX_FORCEINLINE
typename grid_generator<GridFormat>::gb_edge_bitmap* grid_generator<GridFormat>::output_buffer::get_grid_col_bitmap() {
    return _grid_col_bitmap;
}

template <typename GridFormat>
template <typename T>
l0_size_t grid_generator<GridFormat>::output_buffer::get_max_outdegree() const {
    T* required = reinterpret_cast<T*>(_outdegree);
    return *std::max_element(required, required + _outdegree_length);
}

template <typename GridFormat>
template <typename T>
T grid_generator<GridFormat>::output_buffer::get_outdegree_subvector(l1_size_t gb_row) {
    return &reinterpret_cast<T>(_outdegree)[gb_row];
}

template <typename GridFormat>
gstream_error_code grid_generator<GridFormat>::output_buffer::init_outdegree_vector(l0_size_t length) {
    if (length > UINT32_MAX) {
        _outdegree = static_cast<l0_size_t*>(malloc(sizeof(l0_size_t) * length));
        _outdegree_size = sizeof(l0_size_t) * length;
    }
    else if (length > UINT16_MAX) {
        _outdegree = static_cast<l1_size_t*>(malloc(sizeof(l1_size_t) * length));
        _outdegree_size = sizeof(l1_size_t) * length;
    }
    else {
        _outdegree = static_cast<l2_size_t*>(malloc(sizeof(l2_size_t) * length));
        _outdegree_size = sizeof(l2_size_t) * length;
    }
        
    if (_outdegree == nullptr) {
        _outdegree_size = 0;
        return GStreamBadAlloc;
    }
    
    _outdegree_length = length;
    memset(_outdegree, 0, _outdegree_size);
    return GStreamSuccess;
}

template <typename GridFormat>
void grid_generator<GridFormat>::output_buffer::init_bitmap(grid_dim_t const& grid_dim) {
    if (_grid_row_bitmap != nullptr)
        delete[] _grid_row_bitmap;
    _grid_row_bitmap = new gb_edge_bitmap[grid_dim.y];
    // deallocate a column bitmap
    if (_grid_col_bitmap != nullptr)
        delete[] _grid_col_bitmap;
    _grid_col_bitmap = new gb_edge_bitmap[grid_dim.x];
}

template <typename GridFormat>
gstream_error_code grid_generator<GridFormat>::output_buffer::flush_page(FILE* fp) {
    for (auto& page : _page_vec) {
        size_t const r = fwrite(page, page_size, 1, fp);
        if (r != 1)
            return GStreamFileWriteFailure;
    }
    return GStreamSuccess;
}

template <typename GridFormat>
gstream_error_code grid_generator<GridFormat>::output_buffer::flush_gbi(FILE* fp) {
    for (auto& gbi : _gbi_vec) {
        size_t const r = fwrite(gbi, sizeof(gbi_node), 1, fp);
        if (r != 1)
            return GStreamFileWriteFailure;
    }
    return GStreamSuccess;
}

template <typename GridFormat>
gstream_error_code grid_generator<GridFormat>::output_buffer::flush_gipb(FILE* fp) {
    for (auto& gipb : _gipb_vec) {
        size_t const r = fwrite(gipb, sizeof(gb_indirect_pointer_block_physical), 1, fp);
        if (r != 1)
            return GStreamFileWriteFailure;
    }
    return GStreamSuccess;
}

template <typename GridFormat>
gstream_error_code grid_generator<GridFormat>::output_buffer::flush_outdegree(FILE* fp) const {
    size_t const r = fwrite(_outdegree, _outdegree_size, 1, fp);
    if (r != 1)
        return GStreamFileWriteFailure;
    return GStreamSuccess;
}

template <typename GridFormat>
void grid_generator<GridFormat>::output_buffer::_cleanup() {
    // deallocate page buffers
    for (auto& page : _page_vec)
        _page_pool.deallocate(page);
    _page_vec.clear();
    // deallocate gbi nodes
    for (auto& gbi : _gbi_vec)
        _gbi_pool.deallocate(gbi);
    _gbi_vec.clear();
    // deallocate indirect pointer blocks
    for (auto& ib : _gipb_vec)
        _gipb_pool.deallocate(ib);
    _gipb_vec.clear();
    // deallocate a row bitmap
    if (_grid_row_bitmap != nullptr)
        delete[] _grid_row_bitmap;
    _grid_row_bitmap = nullptr;
    // deallocate a column bitmap
    if (_grid_col_bitmap != nullptr)
         delete[] _grid_col_bitmap;
    _grid_col_bitmap = nullptr;
    // deallocate an outdegree vector
    if (_outdegree != nullptr)
        free(_outdegree);
    _outdegree = nullptr;
    _outdegree_size = 0;
    _outdegree_length = 0;
}

} // !namespace grid_format

template <typename GridFormat>
using grid_generator = _grid_format::grid_generator<GridFormat>;

} // !namespace gstream

#ifdef _UNDEF_CRT_SECURE_NO_WARNINGS
#undef _CRT_SECURE_NO_WARNINGS
#undef _UNDEF_CRT_SECURE_NO_WARNINGS
#endif // !_UNDEF_CRT_SECURE_NO_WARNINGS

#ifdef _UNDEF_CRT_SECURE_NO_DEPRECATE
#undef _CRT_SECURE_NO_DEPRECATE
#undef _UNDEF_CRT_SECURE_NO_DEPRECATE
#endif // !_UNDEF_CRT_SECURE_NO_DEPRECATE

#endif // !_GSTREAM_GRID_FORMAT_GRID_GENERATOR_H_
