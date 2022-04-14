#include <iostream>
#include <string>
#include <vector>

#ifndef OS_UNIX
#include <Windows.h>
#endif
class Util {
public:
    static bool IsExistFile(const std::wstring& path);
    static bool IsExistFile(const std::string& path);
    static bool IsExistFile(const std::u16string& path);
    static bool IsWritePermission(const std::string& path);

    static std::string ltrim(const std::string &s);
    static std::string rtrim(const std::string &s);
    static std::string trim(const std::string &s);

    static std::string wstring_to_string(const wchar_t* wstring);
    static std::u16string wstring_to_utf16(const wchar_t* wstring);
    static std::string utf16_to_string(const std::u16string& utf16string);
    static std::u16string string_to_utf16(const std::string& str);
    static std::wstring utf16_to_wstring(const std::u16string& utf16string);
    static std::wstring string_to_wstring(const std::string& str);

    static int createDirectory(const std::u16string& path);

    static void removeDirectory(const char* path);
    static void emptyDirectory(const char* path);
    static bool isDirectory(const char* path);
    static std::string getdir(std::string& dir, std::vector<std::string>& files);
#ifdef OS_UNIX
    static int extract(const char* filePath, const char* unzipPath);
#endif
};