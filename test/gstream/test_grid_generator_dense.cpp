#define _CRT_SECURE_NO_WARNINGS
#include "../grid_format_test.h"
#include <gstream/test/gstream_test_defines.h>
#include <gstream/grid_format/grid_generator.h>
#include <gstream/grid_dataset.h>
#include <mixx/file_system.h>
#include <mixx/string.h>
#include <unordered_map>
#include <unordered_set>

TEST_CASE("grid generator functional (dense)", GSTREAM_TEST_TAG_GRID_FORMAT) {
    using namespace gstream;
    using grid_format = grid_format_template<256, mixx::KiB(1)>;

    char test_dir[FILENAME_MAX], curr_dir[FILENAME_MAX];

    // setup a data path
    {
        mixx::dirname(curr_dir, FILENAME_MAX, __FILE__);
        snprintf(test_dir, FILENAME_MAX, "%s%c..%c%s", curr_dir, MIXX_PATH_SEPARATOR, MIXX_PATH_SEPARATOR, "test_data");
    }

    // make a dense dataset with ground truth
    size_t edge_counter = 0;
    std::unordered_set<uint64_t> vertex_set;
    std::unordered_multimap<uint64_t, uint64_t> edge_set;
    grid_generator<grid_format> gen;
    {
        for(uint64_t row = 0; row < 256; row += 16) {
            vertex_set.insert(row);
            for (uint64_t col = 0; col < 256; col += 16) {
                edge_set.insert(std::make_pair(row, col));
                edge_counter += 1;
                gen.add_edge({row, col});
            } 
        }

        for(uint64_t row = 0; row < 256; row += 8) {
            vertex_set.insert(row);
            for (uint64_t col = 256; col < 512; col += 8) {
                edge_set.insert(std::make_pair(row, col));
                edge_counter += 1;
                gen.add_edge({row, col});
            }
        }

        for (uint64_t row = 256; row < 512; row += 4) {
            vertex_set.insert(row);
            for (uint64_t col = 0; col < 256; col += 4) {
                edge_set.insert(std::make_pair(row, col));
                edge_counter += 1;
                vertex_set.insert(col);
                gen.add_edge({row, col});
            }
        }

        for (uint64_t row = 256; row < 512; row += 2) {
            vertex_set.insert(row);
            for (uint64_t col = 0; col < 512; col += 2) {
                edge_set.insert(std::make_pair(row, col));
                edge_counter += 1;
                vertex_set.insert(col);
                gen.add_edge({row, col});
            }
        }

        gen.generate("syntethic_dense", test_dir);
    }

    in_memory_grid_dataset dataset;
    grid_dataset_info& info = dataset.info;
    REQUIRE(GStreamSuccess == dataset.import("syntethic_dense", test_dir));

    REQUIRE(info.edge_count == edge_counter);
    REQUIRE(info.vertex_count == vertex_set.size());

    // [lambda] validate_edge: validate an (global) edge using ground truth
    auto validate_edge = [&](global_edge const& e) -> bool {
        auto const range = edge_set.equal_range(e.src);
        if (range.first == std::end(edge_set))
            return false;
        bool exists = false;
        for (auto i = range.first; i != range.second; ++i) {
            if (i->second == e.dst)
                exists = true;
        }
        return exists;
    };

    // [lambda] grid_traversal: compare whole edges with ground truth via grid traversal
    auto grid_traversal = [&](grid_format::sparse_gb* gb) {
        unsigned column_iterated = 0;
        unsigned edge_iterated = 0;

        for (auto gb_iter = gb->begin(); gb_iter != gb->end(); ++gb_iter) {
            grid_format::gb_column column = *gb_iter;
            encoded_edge encoded;
            encoded.dst = column.colidx;
            for (auto col_iter = column.begin(); col_iter != column.end(); ++col_iter) {
                encoded.src = *col_iter;
                global_edge e = restore_edge<grid_format>(gb->id, encoded);
                REQUIRE(validate_edge(e) == true);
                edge_iterated += 1;
            }
            column_iterated += 1;
        }
        REQUIRE(gb->column_count == column_iterated);
        REQUIRE(gb->edge_count == edge_iterated);
    };

    for (uint32_t x = 0; x < info.dim.x; ++x) {
        for (uint32_t y = 0; y < info.dim.y; ++y) {
            gbi_node* gbi = dataset.gbi(gbid_t{ x, y });
            if (dataset.gbi(gbid_t{ x, y }) == nullptr)
                continue;
            grid_traversal(static_cast<grid_format::sparse_gb*>(gbi));
        }
    }
}