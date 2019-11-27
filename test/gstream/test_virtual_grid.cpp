// libgstream-test
#include "../grid_format_test.h"
#include <gstream/test/gstream_test_defines.h>
// libgstream
#include <gstream/grid_format.h>
#include <gstream/grid_dataset.h>
#include <gstream/framework/execution_model/virtual_grid_service.h>
// libmixx
#include <mixx/string.h>

using grid_format = as20000102_grid;
using virtual_gb = gstream::virtual_grid_block<grid_format, 4>;

static gstream::global_edge sum;

static void grid_traversal(grid_format::sparse_gb* gb) {
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

static void virtual_grid_traversal(virtual_gb* vgb) {
    for (auto vgb_iter = vgb->begin(); vgb_iter != vgb->end(); ++vgb_iter) {
        grid_format::sparse_gb* gb = *vgb_iter;
        for (auto gb_iter = gb->begin(); gb_iter != gb->end(); ++gb_iter) {
            grid_format::gb_column column = *gb_iter;
            gstream::encoded_edge encoded;
            encoded.dst = column.colidx;
            for (auto col_iter = column.begin(); col_iter != column.end(); ++col_iter) {
                encoded.src = *col_iter;
                sum.src += encoded.src;
                sum.dst += encoded.dst;
                //printf("%u -> %u\n", encoded.src, encoded.dst);
            }
        }
    }
}

TEST_CASE("Virtual grid block functional", GSTREAM_TEST_TAG_GRID_FORMAT) {
    char input_dir[FILENAME_MAX];
    char current_dir[FILENAME_MAX];
    mixx::dirname(current_dir, FILENAME_MAX, __FILE__);
    snprintf(input_dir, FILENAME_MAX, "%s/../%s", current_dir, "test_data");
    gstream::in_memory_grid_dataset dataset;
    REQUIRE(dataset.import("as20000102", input_dir) == gstream::GStreamSuccess);

    auto& info = dataset.info;
    virtual_gb vgb;

    // make a ground truth
    for (uint32_t x = 0; x < info.dim.x; ++x) {
        for (uint32_t y = 0; y < info.dim.y; ++y) {
            gstream::gbi_node* gbi = dataset.gbi(gstream::gbid_t{ x, y });
            if (dataset.gbi(gstream::gbid_t{ x, y }) == nullptr)
                continue; // skip
            grid_traversal(static_cast<grid_format::sparse_gb*>(gbi));
        }
    }
    gstream::global_edge sum1 = sum;
    sum = {0, 0 };

    // run a virtual grid traversal
    for (uint32_t x = 0; x < info.dim.x; ++x) {
        for (uint32_t y = 0; y < info.dim.y; ++y) {
            gstream::gbi_node* gbi = dataset.gbi(gstream::gbid_t{ x, y });
            if (dataset.gbi(gstream::gbid_t{ x, y }) == nullptr)
                continue; // skip
            vgb.add_gbi(gbi);
            if (vgb.size() == 4) {
                virtual_grid_traversal(&vgb);
                vgb.clear();
            }
        }
    }
    if (vgb.size() > 0)
        virtual_grid_traversal(&vgb);
    REQUIRE(sum1 == sum);
}