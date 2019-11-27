#include "../grid_format_test.h"
#include <gstream/test/gstream_test_defines.h>
#include <gstream/grid_dataset.h>
#include <mixx/string.h>

using grid_format = as20000102_grid;

static gstream::global_edge sum;

static void grid_traversal_serial(grid_format::sparse_gb* gb) {
    for (auto gb_iter = gb->begin(); gb_iter != gb->end(); ++gb_iter) {
        grid_format::gb_column column = *gb_iter;
        gstream::encoded_edge encoded;
        encoded.dst = column.colidx;
        for (auto col_iter = column.begin(); col_iter != column.end(); ++col_iter) {
            encoded.src = *col_iter;
            sum.src += encoded.src;
            sum.dst += encoded.dst;
        }
    }
}

static void grid_traversal_interval(grid_format::sparse_gb* gb, gstream::l2_size_t offset, gstream::l2_size_t interval) {
    for (auto gb_iter = gb->begin(offset); gb_iter != gb->end(); gb_iter += interval) {
        grid_format::gb_column column = *gb_iter;
        gstream::encoded_edge encoded;
        encoded.dst = column.colidx;
        for (auto col_iter = column.begin(); col_iter != column.end(); ++col_iter) {
            encoded.src = *col_iter;
            sum.src += encoded.src;
            sum.dst += encoded.dst;
        }
    }
}

TEST_CASE("grid traversal", GSTREAM_TEST_TAG_GRID_FORMAT) {
    char input_dir[FILENAME_MAX];
    char current_dir[FILENAME_MAX];
    mixx::dirname(current_dir, FILENAME_MAX, __FILE__);
    snprintf(input_dir, FILENAME_MAX, "%s/../%s", current_dir, "test_data");

    gstream::in_memory_grid_dataset dataset;
    REQUIRE(dataset.import("as20000102", input_dir) == gstream::GStreamSuccess);

    auto& info = dataset.info;
    for (uint32_t x = 0; x < info.dim.x; ++x) {
        for (uint32_t y = 0; y < info.dim.y; ++y) {
            gstream::gbi_node* gbi = dataset.gbi(gstream::gbid_t{ x, y });
            if (dataset.gbi(gstream::gbid_t{ x, y }) == nullptr)
                continue; // skip
            grid_traversal_serial(static_cast<grid_format::sparse_gb*>(gbi));
        }
    }
    gstream::global_edge sum1 = sum;
    sum = {0, 0 };

    for (uint32_t x = 0; x < info.dim.x; ++x) {
        for (uint32_t y = 0; y < info.dim.y; ++y) {
            gstream::gbi_node* gbi = dataset.gbi(gstream::gbid_t{ x, y });
            if (dataset.gbi(gstream::gbid_t{ x, y }) == nullptr)
                continue; // skip
            grid_traversal_interval(static_cast<grid_format::sparse_gb*>(gbi), 0, 2);
            grid_traversal_interval(static_cast<grid_format::sparse_gb*>(gbi), 1, 2);
        }
    }
    REQUIRE(sum1 == sum);
}