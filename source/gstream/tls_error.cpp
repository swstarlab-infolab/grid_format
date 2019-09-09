#include <gstream/error.h>
#include <mixx/string.h>
#include <stdarg.h>

namespace gstream {

thread_local __tls_error_struct __tls_err;

void print_error(gstream_error const& err) {
    if (err != GStreamSuccess) {
        printf("GStream Error! (error-code: %d, cuda-err: %d)\n"
            "function %s at a line %d in a file: %s\n"
            "message: %s\n",
            err.errc, err.cuda_errc, err.func, err.line, err.file, err.msg.c_str()
        );
    }
    else {
        printf("GStream Success!\n");
    }
}

__tls_error_struct::__tls_error_struct() {
    _err = nullptr;
    _estack = nullptr;
}

__tls_error_struct::~__tls_error_struct() noexcept {
    delete _err;
    delete _estack;
}

void __tls_error_struct::set_success() {
    gstream_error& e = get_mutable();
    //e.file = "NULL";
    //e.line = -1;
    //e.func = "NULL";
    e.errc = GStreamSuccess;
    //e.cuda_errc = -1;
    //e.msg.clear();
}

void __tls_error_struct::set_error(char const* FILE, int LINE, char const* FUNC, errc_t errc) {
    gstream_error& e = get_mutable();
    e.file = mixx::basename(FILE);
    e.line = LINE;
    e.func = FUNC;
    e.errc = errc;
    //e.cuda_errc = -1;
    e.msg.clear();
    
}

void __tls_error_struct::set_cuda_error(char const* FILE, int LINE, char const* FUNC, int cuda_errc) {
    gstream_error& e = get_mutable();
    e.file = mixx::basename(FILE);
    e.line = LINE;
    e.func = FUNC;
    e.errc = GStreamCudaError;
    e.cuda_errc = cuda_errc;
    e.msg.clear();
}

void __tls_error_struct::set_message(std::string const& msg) {
    gstream_error& e = get_mutable();
    e.msg = msg;
}

void __tls_error_struct::set_message(std::string&& msg) {
    gstream_error& e = get_mutable();
    e.msg = std::move(msg);
}

void __tls_error_struct::set_message(char const* format, ...) {
    gstream_error& e = get_mutable();
    if (format == nullptr) {
        e.msg.clear();
        return;
    }
    char buffer[512];
    va_list ap;
    va_start(ap, format);
    vsnprintf(buffer, 512, format, ap);
    va_end(ap);
    e.msg = buffer;
}

void __tls_error_struct::commit() {
    if (_estack == nullptr)
        _estack = new mixx::stack<gstream_error>();
    _estack->push(*_err);
}

} // !namespace gstream