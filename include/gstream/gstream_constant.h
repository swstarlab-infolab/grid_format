#ifndef _GSTREAM_CONSTANT_H_
#define _GSTREAM_CONSTANT_H_

namespace gstream {

enum gstream_error_code: unsigned int {
    GStreamSuccess,
    GStreamUndefined,
    GStreamCudaError,
    GStreamBufferOverflow,
    GStreamFileOpenError,
    GStreamFileWriteError,
    GStreamGridWriteError,
    GStreamGridInfoWriteError,
    GStreamGridInfoNotFound,
    GStreamGridNotFound,
    GStreamGridInfoReadError,
    GStreamGridIndexReadError,
    GStreamGridReadError,
    GStreamBadAlloc,
};

using errc_t = gstream_error_code;

} // !namespace gstream

#endif // _GSTREAM_CONSTANT_H_
