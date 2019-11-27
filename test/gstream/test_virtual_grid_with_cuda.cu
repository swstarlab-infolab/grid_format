#include "../grid_format_test.h"
#include <gstream/test/gstream_test_defines.h>
#include <gstream/framework/execution_model/virtual_grid_service.h>
#include <gstream/grid_dataset.h>
#include <mixx/string.h>
#include <cuda_runtime_api.h>

using grid_format = as20000102_grid;
using virtual_gb = gstream::virtual_grid_block<grid_format, 4>;

static __device__ int device_edge_counter[1];

static __global__ void virtual_grid_traversal(virtual_gb* vgb) {
    unsigned current = 0;
    for (auto vgb_iter = vgb->begin(); vgb_iter != vgb->end(); ++vgb_iter) {
        grid_format::sparse_gb* gb = *vgb_iter;
        for (auto gb_iter = gb->begin(); gb_iter != gb->end(); ++gb_iter) {
            grid_format::gb_column column = *gb_iter;
            //gstream::encoded_edge encoded;
            //encoded.dst = column.colidx;
            for (auto col_iter = column.begin(); col_iter != column.end(); ++col_iter) {
                //encoded.src = *col_iter;
                device_edge_counter[0] += 1;
            }
        }
    }
}

TEST_CASE("virtual grid with cuda", GSTREAM_TEST_TAG_CUDA GSTREAM_TEST_TAG_GRID_FORMAT) {
    char input_dir[FILENAME_MAX];
    char current_dir[FILENAME_MAX];
    mixx::dirname_unix(current_dir, FILENAME_MAX, __FILE__);
    snprintf(input_dir, FILENAME_MAX, "%s/../%s", current_dir, "test_data");

    gstream::in_memory_grid_dataset dataset_host;
    REQUIRE(dataset_host.import("as20000102", input_dir) == gstream::GStreamSuccess);

    gstream::in_memory_device_grid_dataset dataset_dev;
    REQUIRE(dataset_dev.import(dataset_host) == gstream::GStreamSuccess);

    // init counter
    {
        int zero = 0;
        REQUIRE(cudaMemcpyToSymbol(device_edge_counter, &zero, sizeof(int), 0) == cudaSuccess);
    }

    // allocate a deivce vgb buffer
    virtual_gb vgb;
    virtual_gb* vgb_dev;
    {
        REQUIRE(cudaMalloc((void**)&vgb_dev, sizeof(virtual_gb)) == cudaSuccess);
    }

    auto& info = dataset_host.info;
    for (uint32_t x = 0; x < info.dim.x; ++x) {
        for (uint32_t y = 0; y < info.dim.y; ++y) {
            gstream::gbi_node* gbi = dataset_dev.gbi(gstream::gbid_t{ x, y });
            if (dataset_dev.gbi(gstream::gbid_t{ x, y }) == nullptr)
                continue; // skip
            vgb.add_gbi(gbi);
            if (vgb.size() == 4) {
                cudaMemcpy(vgb_dev, &vgb, sizeof(virtual_gb), cudaMemcpyHostToDevice);
                virtual_grid_traversal<<<1,1>>>(vgb_dev);
                vgb.clear();
            }
        }
    }
    REQUIRE(cudaDeviceSynchronize() == cudaSuccess);

    // release the device vgb buffer
    REQUIRE(cudaFree(vgb_dev) == cudaSuccess);

    // test 
    {
        int result = 0;
        REQUIRE( cudaMemcpyFromSymbol(&result, device_edge_counter, sizeof(int), 0) == cudaSuccess);
        REQUIRE( (uint64_t)result == dataset_host.info.edge_count );
        printf("result is %d\n", result);
    }
}