#ifndef _MIXX_FILE_SYSTEM_H_
#define _MIXX_FILE_SYSTEM_H_
#include <mixx/mixx_env.h>

namespace mixx {

bool file_exists(char const* path) noexcept;
mixx_ssize_t get_file_size(char const* path) noexcept;
char* load_file_mm(char* buffer, char const* path) noexcept;
char* load_file_mm(char const* path) noexcept;

} // !namespace mixx

#endif // !_MIXX_FILE_SYSTEM_H_