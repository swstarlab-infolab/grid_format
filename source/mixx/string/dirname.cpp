#include <mixx/string.h>
#include <string.h>

namespace mixx {

char* dirname(char* __restrict buffer, mixx_size_t const bufsize, char const* const __restrict input, char const separator) {
    // init an output buffer
    memset(buffer, 0, bufsize);

    // handle the null string
    if (input == nullptr || input[0] == NULL) {
        buffer[0] = '.';
        return buffer;
    }

    char const* p = input + strlen(input) - 1;
    // trail a last separator
    while (p > input && *p == separator)
        p -= 1;

    // trail a begin of the directory
    while (p > input && *p != separator)
        p -= 1;

    // handle the input is a single separator such as "/" or there ara no slashes
    if (p == input) {
        buffer[0] = *p == separator ? separator : '.';
        return buffer;
    }

    // trail the next separator from back
    do {
        p -= 1;
    } while (p > input &&* p == separator);

    mixx_size_t const len = p - input + 1;
    if (len >= bufsize) {
        // buffer is too small
        return nullptr;
    }
    memcpy(buffer, input, len);

    return buffer;
}

char const* basename(char const* path, char const separator) {
    char const* s = strrchr(path, separator);
    return (s == nullptr) ? path : ++s;
}

char* replace_character(char* s, char const find, char const replace) {
    size_t const len = strlen(s);
    for (size_t i = 0; i < len; ++i) {
        if (s[i] == find)
            s[i] = replace;
    }
    return s;
}

} // !namespace mixx