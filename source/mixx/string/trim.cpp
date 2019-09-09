#include <mixx/string.h>
#include <algorithm>

namespace mixx {

void ltrim(std::string& s, char const c) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [c](char ch) {
        return !(ch == c);
        }));
}

void rtrim(std::string& s, char const c) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [c](char ch) {
        return !(c == ch);
        }).base(), s.end());
}

void trim(std::string& s, char c) {
    ltrim(s, c);
    rtrim(s, c);
}

std::string ltrim_copy(std::string s, char c) {
    ltrim(s, c);
    return s;
}

std::string rtrim_copy(std::string s, char c) {
    rtrim(s, c);
    return s;
}

std::string trim_copy(std::string s, char c) {
    trim(s, c);
    return s;
}

char* trim_path(char* path) {
    char* pos = strrchr(path, MIXX_PATH_SEPARATOR);
    if (pos == (path + (strlen(path) - 1)))
        * pos = 0;
    return path;
}

void trim_path(std::string& path) {
    rtrim(path, MIXX_PATH_SEPARATOR);
}

std::string trim_path_copy(std::string path) {
    trim_path(path);
    return path;
}

} // !namespace mixx