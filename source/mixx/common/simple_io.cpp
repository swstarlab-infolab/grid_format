#include <mixx/file_system.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

namespace mixx {

bool file_exists(char const* path) noexcept {
#if defined(MIXX_TOOLCHAIN_MSVC)
    struct _stat64 buf;
    return (__stat64(path, &buf) == 0);
#elif defined(MIXX_TOOLCHAIN_GNUC)
    struct stat64 buf;
    return (stat64(path, &buf) == 0);
#endif
}

mixx_ssize_t get_file_size(char const* path) noexcept {
#if defined(MIXX_TOOLCHAIN_MSVC)
    struct _stat64 st;
    if (__stat64(path, &st) == -1)
        return -1;
    return st.st_size;
#elif defined(MIXX_TOOLCHAIN_GNUC)
    struct stat64 st;
    if (stat64(path, &st) == -1)
        return -1;
    return st.st_size;
#else
#error NOT SUPPORTED PLATFORM
#endif
}

char* load_file_mm(char* buffer, char const* path) noexcept {
    bool err = false;
    const mixx_ssize_t file_size = get_file_size(path);
    FILE* fp;
    do {
        fopen_s(&fp, path, "rb");
        if (fp == nullptr) {
            fprintf(stderr, "failed to open a file \'%s\'", path);
            err = true;
            break;
        }
        const size_t r = fread(buffer, file_size, 1, fp);
        if (r < 1) {
            fprintf(stderr, "failed to read file \'%s\'! (%zu / %" PRIu64 "); ferror() returned: %d\n", path, r, 1llu, ferror(fp));
            err = true;
        }
        fclose(fp);
    } while (false);
    if (!err)
        return buffer;
    return nullptr;
}

char* load_file_mm(char const* path) noexcept {
    if (!file_exists(path)) {
        fprintf(stderr, "a file \'%s\' does not exists.", path);
        return nullptr;
    }

    const mixx_ssize_t file_size = get_file_size(path);
    char* buffer = static_cast<char*>(calloc(1, file_size));
    if (buffer == nullptr) {
        fprintf(stderr, "failed to allocate a buffer for loading \'%s\'", path);
        return nullptr;
    }
    bool err = false;
    FILE* fp;
    do {
        fopen_s(&fp, path, "rb");
        if (fp == nullptr) {
            fprintf(stderr, "failed to open a file \'%s\'", path);
            err = true;
            break;
        }
        const size_t r = fread(buffer, file_size, 1, fp);
        if (r < 1) {
            fprintf(stderr, "failed to read file \'%s\'! (%zu / %" PRIu64 "); ferror() returned: %d\n", path, r, 1llu, ferror(fp));
            err = true;
        }
        fclose(fp);
    } while (false);
    if (!err)
        return buffer;
    free(buffer);
    return nullptr;
}

} // !namespace mixx
