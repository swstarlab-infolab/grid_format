#ifndef _GSTREAM_GRID_FORMAT_GENERATOR_GRID_RESOURCE_BUILDER_H_
#define _GSTREAM_GRID_FORMAT_GENERATOR_GRID_RESOURCE_BUILDER_H_

#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#define _UNDEF_CRT_SECURE_NO_DEPRECATE
#endif // !_CRT_SECURE_NO_DEPRECATE

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#define _UNDEF_CRT_SECURE_NO_WARNINGS
#endif // !_CRT_SECURE_NO_WARNINGS

#include <gstream/grid_format/internal_defines.h>
#include <mixx/detail/noncopyable.h>
#include <mixx/pool.h>
#include <assert.h>
#include <limits>
#include <unordered_map>

namespace gstream {
namespace _grid_format {

using gbr_node_id = l0_size_t;

template <typename GridFormat, unsigned ChunkSize>
class gb_resource_node: mixx::noncopyable {
public:
    GSTREAM_ALIAS_GRID_FORMAT_INTERNAL_DEFINES(GridFormat);

    struct header_t {
        gbr_node_id id;
        gbr_node_id prev;
        gbid_t gbid;
        unsigned size;
        l0_size_t global_col_off;
        l0_size_t global_row_off;
    };
    static constexpr unsigned chunk_size = ChunkSize;
    static constexpr unsigned data_section_size = chunk_size - sizeof(header_t);
    static constexpr unsigned capacity = data_section_size / sizeof(encoded_edge);

    gb_resource_node() = default;
    ~gb_resource_node() noexcept = default;
    
    void add_edge(global_edge const& e) {
        assert(/* boundary checking */ e.src >= _header.global_row_off && e.src < (_header.global_row_off + gb_width));
        assert(/* boundary checking */e.dst >= _header.global_col_off && e.dst < (_header.global_col_off + gb_width));
        assert(/* overflow checking */ (e.src - _header.global_row_off) <= UINT16_MAX );
        assert(/* overflow checking */ (e.dst - _header.global_col_off) <= UINT16_MAX );
        assert(is_full() == false);
        encoded_edge& encoded = _encoded(_header.size - 1);
        encoded.src = static_cast<encoded_vid>(e.src - _header.global_row_off);
        encoded.dst = static_cast<encoded_vid>(e.dst - _header.global_col_off);
        _header.size += 1;
    }
    
    void reset(l0_size_t const id, gbid_t const gbid) {
        _header.id = id;
        _header.prev = std::numeric_limits<l0_size_t>::max();
        _header.gbid = gbid;
        _header.size = 0;
        _header.global_col_off = 0;
        _header.global_row_off = 0;
        memset(_data, 0, data_section_size);
    }

    void set_prev(l0_size_t const prev_id) {
        _header.prev = prev_id;
    }

    l0_size_t id() const {
        return _header.id;
    }

    size_t size() const {
        return _header.size;
    }

    bool is_full() const {
        return _header.size == capacity;
    }

protected:
    encoded_edge& _encoded(int offset) {
        return static_cast<encoded_edge*>(_data)[offset];
    }

    header_t _header;
    char _data[data_section_size];
};

template <typename GBRNode>
class gb_resourece_buffer {
public:
    using gbr_node_t = GBRNode;
    GSTREAM_ALIAS_GRID_FORMAT_INTERNAL_DEFINES(typename gbr_node_t::grid_format);
    void archive(gbr_node_t*);
    gbr_node_t* allocate();
protected:
    struct gbr_index {

    };

    mixx::object_pool<gbr_node_t> _node_pool;
    
};

template <typename GBRNode>
class gb_resource_builder {
    using gbr_node_t = GBRNode;
    GSTREAM_ALIAS_GRID_FORMAT_INTERNAL_DEFINES(typename gbr_node_t::grid_format);
    using gbr_map_t = std::unordered_map<uint64_t, gbr_node_t*>;
};

} // !namespace _grid_format
} // !namespace gstream

#endif // !_GSTREAM_GRID_FORMAT_GENERATOR_GRID_RESOURCE_BUILDER_H_