#define _CRT_SECURE_NO_WARNINGS
#include "../grid_format_test.h"
#include <gstream/test/gstream_test_defines.h>
#include <gstream/grid_format/grid_generator.h>
#include <gstream/grid_dataset.h>
#include <mixx/file_system.h>
#include <mixx/string.h>
#include <unordered_map>
#include <unordered_set>

template <typename Edge>
static char* tsv_get_next_edge(Edge&& e, char* p) {
    char* q;
    while (true) {
        q = strchr(p, '\n');
        if (q != nullptr)
            *q = 0;
        if (*p != '#') {
            const int n = sscanf(p, "%zu %zu", &e.src, &e.dst);
            if (n == 2) {
                return q + 1;
            }
            return nullptr;
        }
        p = q + 1;
    }
}

TEST_CASE("grid generator functional (sparse)", GSTREAM_TEST_TAG_GRID_FORMAT) {
    using namespace gstream;
    using grid_format = as20000102_grid;

    char path[FILENAME_MAX], test_dir[FILENAME_MAX], curr_dir[FILENAME_MAX];

    // setup a data path
    {
        mixx::dirname(curr_dir, FILENAME_MAX, __FILE__);
        snprintf(test_dir, FILENAME_MAX, "%s%c..%c%s", curr_dir, MIXX_PATH_SEPARATOR, MIXX_PATH_SEPARATOR, "test_data");
        snprintf(path, FILENAME_MAX, "%s%c" "as20000102.tsv", test_dir, MIXX_PATH_SEPARATOR);
    } 

    // setup the ground truth
    size_t edge_counter = 0;
    std::unordered_set<uint64_t> vertex_set;
    std::unordered_multimap<uint64_t, uint64_t> edge_set;
    { 
        // load file into memory
        char* input_tsv = mixx::load_file_into_memory(path);
        REQUIRE(input_tsv != nullptr);
        // generate a ground truth
        char* pos = input_tsv;
        do {
            global_edge e;
            pos = tsv_get_next_edge(e, pos);
            if (pos == nullptr)
                break;
            //if (e.src == e.dst) continue;
            //printf("edge %llu: %llu -> %llu\n", edge_counter, e.src, e.dst);
            edge_set.insert(std::make_pair(e.src, e.dst));
            vertex_set.insert(e.src);
            vertex_set.insert(e.dst);
            edge_counter += 1;
        } while (true);

        // release input buffer
        free(input_tsv);
    }

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

    SECTION("grid generation") {
        grid_generator<grid_format> gen;
        for (auto& kv : edge_set) {
            global_edge e;
            e.src = kv.first;
            e.dst = kv.second;
            gen.add_edge(e);
        }
        REQUIRE(GStreamSuccess == gen.generate("as20000102", test_dir));
    }

    SECTION("validation") {
        in_memory_grid_dataset dataset;
        grid_dataset_info& info = dataset.info;
        REQUIRE(GStreamSuccess == dataset.import("as20000102", test_dir));

        REQUIRE(info.edge_count == edge_counter);
        REQUIRE(info.vertex_count == vertex_set.size());

        for (uint32_t x = 0; x < info.dim.x; ++x) {
            for (uint32_t y = 0; y < info.dim.y; ++y) {
                gbi_node* gbi = dataset.gbi(gbid_t{ x, y });
                if (dataset.gbi(gbid_t{ x, y }) == nullptr)
                    continue;
                grid_traversal(static_cast<grid_format::sparse_gb*>(gbi));
            }
        }
    }
}
