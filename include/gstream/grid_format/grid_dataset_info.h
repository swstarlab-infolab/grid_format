/*!
 * GStream project by InfoLab @ DGIST (https://infolab.dgist.ac.kr)
 * Copyright 2019 GStream Authors. All Rights Reserved.
 * 
 * <License>
 * 
 * @file gstream/grid_format/grid_dataset_info.h
 * @project LibGStream
 * @brief A metadata of the grid dataset
 * @author Seyeon Oh (mailto:vee@dgist.ac.kr)
 * @version 1.0 4/25/2019
 */
#ifndef _GSTREAM_GRID_FORMAT_GRID_DATASET_INFO_H_
#define _GSTREAM_GRID_FORMAT_GRID_DATASET_INFO_H_
#include <mixx/dimension.h>
#include <stdint.h>

namespace gstream {

#define GSTREAM_DATASET_NAME_MAX 128

struct grid_dataset_info {
    char name[GSTREAM_DATASET_NAME_MAX];
    char toolchain_name[GSTREAM_DATASET_NAME_MAX];
    uint32_t gb_width;  // grid block width
    uint32_t page_size;
    mixx::dim2<uint32_t> dim;
    uint64_t gbi_count;    // gbi: grid block indexing node
    uint64_t blank_count;  // blank grid block count
    uint64_t sgbi_count;   // sgbi: small gbi 
    uint64_t gipb_count;    // gipb: grid indirect pointer block
    uint64_t cb_count;     // cell block count
    uint64_t sparse_count; // sparse grid block count
    uint64_t dense_count;  // dense grid block count
    uint64_t page_count;
    uint64_t vertex_count;
    uint64_t max_vid;
    uint64_t max_outdegree;
    uint64_t edge_count;
    uint32_t version;
    uint64_t topology_size() const;
    void printout() const;
};

} // !namespace gstream

#endif // !_GSTREAM_GRID_FORMAT_GRID_DATASET_INFO_H_
