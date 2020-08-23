#include "../grid_format_test.h"
#include <gstream/test/gstream_test_defines.h>
#include <gstream/grid_format.h>
#include <gstream/grid_format/grid_generator.h>

TEST_CASE("Grid format building block functional", GSTREAM_TEST_TAG_GRID_FORMAT) {
    using namespace gstream;
    using grid_format = as20000102_grid;
    using cbb_type = _grid_format::cb_buliding_block<grid_format>;

    SECTION("grid block building resource container") {
        gbid_t gbid { 0, 0 };
        _grid_format::gb_resource_container<grid_format> gbr(gbid);
        global_edge e;
        for (l2_size_t i = 0; i < grid_format::gb_width; i += 4) {
            for (l2_size_t j = 0; j < grid_format::gb_width; j += 2) {
                 e.src = grid_format::gb_width - 1- j;
                 e.dst = grid_format::gb_width - 1- i;
                 gbr.add_edge(e);
            }
        }
        REQUIRE(gbr.encoded_edges.size() == (grid_format::gb_width / 4) * (grid_format::gb_width / 2));
        REQUIRE(gbr.col_bitmap.count() == (grid_format::gb_width / 4));
        REQUIRE(gbr.row_bitmap.count() == (grid_format::gb_width / 2));

        gbr.sort();

        l1_size_t offset = 0;
        for (l2_size_t i = 0; i < grid_format::gb_width; i += 4) {
            for (l2_size_t j = 0; j < grid_format::gb_width; j += 2) {
                 REQUIRE(gbr.encoded_edges[offset].src == j + 1);
                 REQUIRE(gbr.encoded_edges[offset].dst == i + 3);
                 offset += 1;
            }
        }
    }

    SECTION("cell block builing block") {
        cbb_type cbb;
        l1_size_t free = mixx::KiB(4) - _grid_format::cell_block::dummy_size - sizeof(_grid_format::cell_block_header);
        REQUIRE(cbb.scan() == 0);
        REQUIRE(cbb.is_empty() == true);

        cbb.reset(mixx::KiB(4));
        REQUIRE(cbb.free.get() == free);

        l1_size_t scan_result, edge_count;
        edge_count = scan_result = cbb.scan();
        REQUIRE(scan_result == grid_format::gb_width + 0); // + 0 for preventing nvcc linking issue...
        cbb.put_column(scan_result);

        edge_count += scan_result = cbb.scan();
        REQUIRE(scan_result != grid_format::gb_width + 0); // + 0 for preventing nvcc linking issue...
        cbb.put_column(scan_result);

        REQUIRE(cbb.is_empty() == false);
        REQUIRE(cbb.scan() == 0u);
        REQUIRE(cbb.free.get() == 0u);
        REQUIRE(cbb.edge_count.get() == edge_count);
        REQUIRE(cbb.column_count.get() == 2u);
        REQUIRE(cbb.size_ptrs.get() == 12u);
        REQUIRE(cbb.size_rows.get() == 2u * edge_count);
        REQUIRE(cbb.size_cols.get() == 4u);

        free = mixx::KiB(4) - _grid_format::cell_block::dummy_size;
        cbb.reset(mixx::KiB(4));
    }

    SECTION("page building block") {
        _grid_format::page_building_block<grid_format> pbb;
        l1_size_t free = grid_format::page_size - sizeof(_grid_format::grid_page_header);
        l1_size_t index_size = 0;
        REQUIRE(pbb.scan() == free - sizeof(l1_size_t));
        REQUIRE(pbb.next_cb_offset() == grid_format::page_size - free - index_size);
        pbb.put_cell_block(512);
        free -= 512 + sizeof(l1_size_t);
        index_size += sizeof(l1_size_t);
        REQUIRE(pbb.scan() == free - sizeof(l1_size_t));
        REQUIRE(pbb.next_cb_offset() == grid_format::page_size - free - index_size);
    }
}