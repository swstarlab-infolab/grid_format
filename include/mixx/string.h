#ifndef _MIXX_STRING_H_
#define _MIXX_STRING_H_
#include <mixx/mixx_env.h>
#include <string>

namespace mixx {

char* dirname(char* __restrict buffer, mixx_size_t bufsize, char const* __restrict input, char separator);
void dirname(std::string& s, char separator);
std::string dirname_copy(std::string const& s, char separator);
char const* basename(char const* path, char separator = MIXX_PATH_SEPARATOR);
char* replace_character(char* s, char find, char replace);

void ltrim(std::string& s, char c = 0x20);
void rtrim(std::string& s, char c = 0x20);
void trim(std::string& s, char c = 0x20);
std::string ltrim_copy(std::string s, char c = 0x20);
std::string rtrim_copy(std::string s, char c = 0x20);
std::string trim_copy(std::string s, char c = 0x20);

char* trim_path(char* path);
void trim_path(std::string& path);
std::string trim_path_copy(std::string path);

} // !namespace mixx
#endif // _MIXX_STRING_H_
