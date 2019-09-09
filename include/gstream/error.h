#ifndef _GSTREAM_ERROR_H_
#define _GSTREAM_ERROR_H_
#include <gstream/gstream_constant.h>
#include <mixx/stack.h>
#include <string>

namespace gstream {

struct gstream_error {
    char const* file = "NULL";
    int line = -1;
    char const* func = "NULL";
    errc_t errc = GStreamUndefined;
    int    cuda_errc = -1;
    std::string msg;

    operator bool() const {
        return errc != GStreamSuccess;
    }
};

void print_error(gstream_error const& err);

class __tls_error_struct {
public:
    __tls_error_struct();
    ~__tls_error_struct() noexcept;
    void set_success();
    void set_error(char const* FILE, int LINE, char const* FUNC, errc_t errc);
    void set_cuda_error(char const* FILE, int LINE, char const* FUNC, int cuda_errc);
    void set_message(std::string const& msg);
    void set_message(std::string&& msg);
    void set_message(char const* format = nullptr, ...);
    void commit();

    MIXX_FORCEINLINE gstream_error const& get() {
        return get_mutable();
    }

private:
    MIXX_FORCEINLINE gstream_error& get_mutable() {
        if (MIXX_LIKELY(_err != nullptr))
            return *_err;
        _err = new gstream_error;
        return *_err;
    }

    mixx::stack<gstream_error>* _estack;
    gstream_error* _err;
};

extern thread_local __tls_error_struct __tls_err;

inline gstream_error const& get_last_error() {
    return __tls_err.get();
}

#define _GSTREAM_TLS_ERROR_LOG(ErrorCode, ...) \
__tls_err.set_error(__FILE__, __LINE__, __FUNCTION__, ErrorCode);\
__tls_err.set_message(__VA_ARGS__);\
__tls_err.commit()

#define _GSTREAM_TLS_CUDA_ERROR_LOG(CudaErr, ...) \
__tls_err.set_cuda_error(__FILE__, __LINE__, __FUNCTION__, CudaErr);\
__tls_err.set_message(__VA_ARGS__);\
__tls_err.commit()

#define _GSTREAM_CUDA_API_CALL(EXPR) \
{ cudaError_t __cuda_err = (EXPR);\
    if (__cuda_err != cudaSuccess)\
        _GSTREAM_TLS_CUDA_ERROR_LOG(__cuda_err, #EXPR);\
    _GSTREAM_SET_TLS_ERROR_SUCCESS();\
}

} // !namespace gstream

#endif // !_GSTREAM_ERROR_H_
