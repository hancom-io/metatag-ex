#ifndef _METATAGEX_UTIL_
#define _METATAGEX_UTIL_

#include <Windows.h>
#include <string>
#include <vector>


class Util {
public:
    static std::string wstring_to_string(const wchar_t* wstring);
    static std::u16string wstring_to_utf16(const wchar_t* wstring);
    static std::string utf16_to_string(const std::u16string& utf16string);
    static std::u16string string_to_utf16(const std::string& str);
    static std::wstring utf16_to_wstring(const std::u16string& utf16string);
    static std::wstring string_to_wstring(const std::string& str);

    static bool isDirectory(const char* path);
    static std::string getdir(std::string& dir, std::vector<std::string>& files);

};

#endif //_METATAGEX_UTIL_