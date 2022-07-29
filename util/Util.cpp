#include <atlstr.h>
#include "./Util.h"

std::string Util::wstring_to_string(const wchar_t* wstring) {
    char strMultibyte[256] = { 0, };
    int len = WideCharToMultiByte(CP_ACP, 0, wstring, -1, NULL, 0, NULL, NULL);
    WideCharToMultiByte(CP_ACP, 0, wstring, -1, strMultibyte, len, NULL, NULL);

    return strMultibyte;
}

std::u16string Util::wstring_to_utf16(const wchar_t* wstring) {
    return Util::string_to_utf16(Util::wstring_to_string(wstring));
}

std::wstring Util::utf16_to_wstring(const std::u16string& utf16string)
{
    std::wstring wstrTemp;
    std::string strTemp = Util::utf16_to_string(utf16string);
    wstrTemp = Util::string_to_wstring(strTemp);

    return wstrTemp;
}

std::string Util::utf16_to_string(const std::u16string& utf16string) {
    CStringA utf8;
    int len = WideCharToMultiByte(CP_ACP, 0, (LPCWCH)utf16string.c_str(), -1, NULL, 0, 0, 0);
    if (len > 1)
    {
        char *ptr = utf8.GetBuffer(len - 1);
        if (ptr) WideCharToMultiByte(CP_ACP, 0, (LPCWCH)utf16string.c_str(), -1, ptr, len, 0, 0);
        utf8.ReleaseBuffer();
    }
    return utf8;
}

std::wstring Util::string_to_wstring(const std::string& str)
{
    int strLength = (int)str.length() + 1;
    int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), strLength, 0, 0);

    wchar_t* pBuf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), strLength, pBuf, len);
    std::wstring res(pBuf);
    delete[] pBuf;
    return res;
}

std::string Util::getdir(std::string& dir, std::vector<std::string>& files)
{
    char fullPath[MAX_PATH];
    char* fileName;
    GetFullPathName(dir.c_str(), MAX_PATH, fullPath, &fileName);
    dir = std::string(fullPath);
    std::string regexStr = "";
    if (Util::isDirectory(dir.c_str()) == false)
    {
        regexStr = dir.substr(dir.find_last_of("\\") + 1, dir.length() - dir.find_last_of("\\"));
        dir = dir.substr(0, dir.find_last_of("\\"));
    }
    char search_path[200];
    sprintf_s(search_path, "%s\\*.*", dir.c_str());
    WIN32_FIND_DATA fd;
    HANDLE hFind = ::FindFirstFile(search_path, &fd);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                files.push_back(fd.cFileName);
            }
        } while (::FindNextFile(hFind, &fd));
        ::FindClose(hFind);
    }
    return regexStr;
}

std::u16string Util::string_to_utf16(const std::string& str)
{
    std::string strTemp = CW2A(CA2W(str.c_str()), CP_UTF8);
    std::vector<unsigned long> unicode;
    size_t i = 0;
    while (i < strTemp.size())
    {
        unsigned long uni;
        size_t todo;
        // bool error = false;
        unsigned char ch = strTemp[i++];
        if (ch <= 0x7F)
        {
            uni = ch;
            todo = 0;
        } else if (ch <= 0xBF)
        {
            throw "not a UTF-8 string";
        } else if (ch <= 0xDF)
        {
            uni = ch & 0x1F;
            todo = 1;
        } else if (ch <= 0xEF)
        {
            uni = ch & 0x0F;
            todo = 2;
        } else if (ch <= 0xF7)
        {
            uni = ch & 0x07;
            todo = 3;
        } else
        {
            throw "not a UTF-8 string";
        }
        for (size_t j = 0; j < todo; ++j)
        {
            if (j == strTemp.size())
                throw "not a UTF-8 string";
            unsigned char ch = strTemp[i++];
            if (ch < 0x80 || ch > 0xBF)
                throw "not a UTF-8 string";
            uni <<= 6;
            uni += ch & 0x3F;
        }
        if (uni >= 0xD800 && uni <= 0xDFFF)
            throw "not a UTF-8 string";
        if (uni > 0x10FFFF)
            throw "not a UTF-8 string";
        unicode.push_back(uni);
    }

    std::u16string utf16;
    for (size_t i = 0; i < unicode.size(); ++i)
    {
        unsigned long uni = unicode[i];
        if (uni <= 0xFFFF)
        {
            utf16 += (char16_t)uni;
        } else
        {
            uni -= 0x10000;
            utf16 += (char16_t)((uni >> 10) + 0xD800);
            utf16 += (char16_t)((uni & 0x3FF) + 0xDC00);
        }
    }

    return utf16;
}

bool Util::isDirectory(const char* path)
{
    if (path == NULL) {
        return false;
    }

    DWORD attr = GetFileAttributes(path);
    if (attr == INVALID_FILE_ATTRIBUTES) {
        return false;
    }

    if (attr & FILE_ATTRIBUTE_DIRECTORY) {
        return true;
    }

    return false;
}
