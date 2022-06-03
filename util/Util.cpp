#ifdef OS_UNIX
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <archive.h>
#include <archive_entry.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#else
#include <Windows.h>
#include <atlstr.h>
#include <stdio.h>
#include <io.h>
#include <comutil.h>
#include <ShlDisp.h>
#include <codecvt>
#include <locale>
#include <direct.h>
#ifdef _DEBUG

# pragma comment(lib, "comsuppwd.lib")

#else

# pragma comment(lib, "comsuppw.lib")

#endif

#endif // OS_UNIX
#include <sys/stat.h>
#include <locale>
#include <vector>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstdio>
#include "./Util.h"

#ifdef OS_UNIX
int Util::extract(const char* filePath, const char* unzipPath)
{
    //#ifdef OS_UNIX
    struct archive_entry *entry;
    int result = 0;
    int status;

    struct archive* aFrom = archive_read_new();
    if (!aFrom)
        return -1;

    if (archive_read_support_filter_all(aFrom) != ARCHIVE_OK)
        return -1;

    if (archive_read_support_format_all(aFrom) != ARCHIVE_OK)
        return -1;

    int r = archive_read_open_filename(aFrom, filePath, 10240);
    if (r != ARCHIVE_OK)
        return -1;

    int flags = ARCHIVE_EXTRACT_PERM
        | ARCHIVE_EXTRACT_TIME
        | ARCHIVE_EXTRACT_ACL
        | ARCHIVE_EXTRACT_FFLAGS
        | ARCHIVE_EXTRACT_XATTR;


    /* Avoid spurious warnings.  One should test for the CAP_CHOWN
     * capability instead but libarchive only does this test: */
    if (geteuid() == 0)
        flags |= ARCHIVE_EXTRACT_OWNER;

    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    chdir(unzipPath);

    while (archive_read_next_header(aFrom, &entry) == ARCHIVE_OK) {
        status = archive_read_extract(aFrom, entry, flags);
        switch (status) {
        case ARCHIVE_OK:
            break;

        default:
            result = -1;
            break;
        }
    }

    chdir(cwd);

    archive_read_close(aFrom);
    archive_read_free(aFrom);

    return result;
}
#endif

bool Util::IsExistFile(const std::wstring& path)
{
    // 파일 존재 확인
    // access 에선 wchar_t 를 소화하지 못하여 multibyte 로 임시 변경하여 사용
    return IsExistFile(wstring_to_string(path.c_str()));
}

bool Util::IsExistFile(const std::string& path)
{
#ifdef OS_UNIX    
    if (access(path.c_str(), F_OK) < 0) {
#else
#define	F_OK	0		/* Test for existence.  */

    if (_access(path.c_str(), F_OK) < 0) {
#endif
        return false;
    }

    return true;
}

bool Util::IsExistFile(const std::u16string& path)
{
    return IsExistFile(utf16_to_string(path));
}

bool Util::IsWritePermission(const std::string& path)
{
#ifdef OS_UNIX
    return (access(path.c_str(), W_OK) < 0) ? false : true;
#else
#define	W_OK	2		/* Test for write permission.  */

    return (_access(path.c_str(), W_OK) < 0) ? false : true;
#endif
}


const std::string WHITESPACE = " \n\r\t\f\v";
std::string Util::ltrim(const std::string &s)
{
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}

std::string Util::rtrim(const std::string &s)
{
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string Util::trim(const std::string &s) {
    return rtrim(ltrim(s));
}

std::string Util::wstring_to_string(const wchar_t* wstring) {
    // sizeof (char16_t) == (wchar_t) 일때
#ifdef OS_UNIX
    return utf16_to_string(reinterpret_cast<const char16_t*>(wstring));
#else
    char strMultibyte[256] = { 0, };
    int len = WideCharToMultiByte(CP_ACP, 0, wstring, -1, NULL, 0, NULL, NULL);
    WideCharToMultiByte(CP_ACP, 0, wstring, -1, strMultibyte, len, NULL, NULL);

    return strMultibyte;
#endif
}

std::u16string Util::wstring_to_utf16(const wchar_t* wstring) {
#ifdef OS_UNIX
    // sizeof (char16_t) == (wchar_t) 일때
    if (sizeof(char16_t) == sizeof(wchar_t)) {
        return reinterpret_cast<const char16_t*>(wstring);
    }
#else
    return Util::string_to_utf16(Util::wstring_to_string(wstring));
#endif
    return u"";
}

std::wstring Util::utf16_to_wstring(const std::u16string& utf16string)
{
#ifdef OS_UNIX
    // sizeof (char16_t) == (wchar_t) 일때
    if (sizeof(char16_t) == sizeof(wchar_t)) {
        return reinterpret_cast<const wchar_t*>(utf16string.c_str());
    }
#else
    std::wstring wstrTemp;
    std::string strTemp = Util::utf16_to_string(utf16string);
    wstrTemp = Util::string_to_wstring(strTemp);

    return wstrTemp;
#endif

    return L"";
}

std::string Util::utf16_to_string(const std::u16string& utf16string) {
#ifdef OS_UNIX
    std::string utf8String;

    int size = 0;

    for (char16_t utf16char : utf16string) {
        char convertedUtf8[5] = { 0x00, };

        if (utf16char < 0x80) {
            convertedUtf8[0] = (utf16char >> 0 & 0x7F) | 0x00;
            size += 1;
        } else if (utf16char < 0x0800) {
            convertedUtf8[0] = (utf16char >> 6 & 0x1F) | 0xC0;
            convertedUtf8[1] = (utf16char >> 0 & 0x3F) | 0x80;
            size += 2;
        } else if (utf16char < 0x010000) {
            convertedUtf8[0] = (utf16char >> 12 & 0x0F) | 0xE0;
            convertedUtf8[1] = (utf16char >> 6 & 0x3F) | 0x80;
            convertedUtf8[2] = (utf16char >> 0 & 0x3F) | 0x80;
            size += 3;
        } else if (utf16char < 0x110000) {
            convertedUtf8[0] = (utf16char >> 18 & 0x07) | 0xF0;
            convertedUtf8[1] = (utf16char >> 12 & 0x3F) | 0x80;
            convertedUtf8[2] = (utf16char >> 6 & 0x3F) | 0x80;
            convertedUtf8[3] = (utf16char >> 0 & 0x3F) | 0x80;
            size += 4;
        }

        utf8String.append(convertedUtf8);
    }

    return utf8String;
#else
    CStringA utf8;
    int len = WideCharToMultiByte(CP_ACP, 0, (LPCWCH)utf16string.c_str(), -1, NULL, 0, 0, 0);
    if (len > 1)
    {
        char *ptr = utf8.GetBuffer(len - 1);
        if (ptr) WideCharToMultiByte(CP_ACP, 0, (LPCWCH)utf16string.c_str(), -1, ptr, len, 0, 0);
        utf8.ReleaseBuffer();
    }
    return utf8;
#endif
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
#ifdef OS_UNIX
    DIR *dp;
    std::string regexStr = "";
    struct dirent *dirp;
    if (Util::isDirectory(dir.c_str()) == false)
    {
        regexStr = dir.substr(dir.find_last_of("/") + 1, dir.length() - dir.find_last_of("/"));
        dir = dir.substr(0, dir.find_last_of("/"));
    }
    if ((dp = opendir(dir.c_str())) == NULL)
    {
        std::cout << "Error" << std::endl;
        return regexStr;
    }
    while ((dirp = readdir(dp)) != NULL)
    {
        files.push_back(std::string(dirp->d_name));
    }
    closedir(dp);
    return regexStr;
#else // // OS_UNIX
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
#endif
}

std::u16string Util::string_to_utf16(const std::string& str)
{
#ifdef OS_UNIX
    std::string strTemp = str;
#else
    std::string strTemp = CW2A(CA2W(str.c_str()), CP_UTF8);
#endif
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

#ifdef OS_UNIX
int Util::createDirectory(const std::u16string& path)
{
    return mkdir(Util::utf16_to_string(path).c_str(), 0777);    // 7 : 읽기(4) 쓰기(2) 검색(1) 로 각각 Owner, Group, Other 에 권한을 준다.
}
#endif

void Util::removeDirectory(const char* path)
{
    emptyDirectory(path);

#ifdef OS_UNIX
    if (access(path, W_OK) != 0) {
        chmod(path, S_IRWXU);
    }

    rmdir(path);
#else
    std::string dirC(path);
    dirC += "\\";
    WIN32_FIND_DATA finddata;
    std::string dirTemp(dirC);
    dirTemp += "*.*";
    HANDLE filehandle = FindFirstFile(dirTemp.c_str(), &finddata);
    if (filehandle != INVALID_HANDLE_VALUE)
    {
        do
        {
            std::string Lfilename(dirC);
            Lfilename += finddata.cFileName;
            if (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if (std::string(finddata.cFileName).compare(".") != 0 && std::string(finddata.cFileName).compare("..") != 0)
                {
                    removeDirectory(Lfilename.c_str());
                    RemoveDirectory(Lfilename.c_str());
                }
            } else
            {
                DeleteFile(Lfilename.c_str());
            }
        } while (FindNextFile(filehandle, &finddata));
        FindClose(filehandle);
    }
    RemoveDirectory(path);
#endif
}

void Util::emptyDirectory(const char* path)
{
    if (isDirectory(path) == false) {
        return;
    }

#ifdef OS_UNIX
    DIR* dir;
    struct dirent *entry;

    dir = opendir(path);
    if (dir == NULL) {
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            std::string delPath = path;
            delPath += "/";
            delPath += entry->d_name;

            if (entry->d_type == DT_DIR) {
                removeDirectory(delPath.c_str());
            } else {
                std::remove(delPath.c_str());
            }
        }
    }

    closedir(dir);
#else
    HANDLE handle;
    WIN32_FIND_DATA data;

    handle = FindFirstFile(path, &data);
    if (handle == INVALID_HANDLE_VALUE) {
        RemoveDirectory(path);
        return;
    }

    BOOL result = true;
    while (result) {
        std::string buffer = std::string(path) + std::string(data.cFileName);

        if (strcmp(data.cFileName, ".") && strcmp(data.cFileName, "..")) {
            if (isDirectory(buffer.c_str()) == true) {
                removeDirectory(buffer.c_str());
            } else {
                remove(buffer.c_str());
            }
        }

        result = FindNextFile(handle, &data);
    }

    FindClose(handle);
#endif
}

bool Util::isDirectory(const char* path)
{
    if (path == NULL) {
        return false;
    }

    if (IsExistFile(path) == false) {
        return false;
    }

#ifdef OS_UNIX
    struct stat result;
    if (stat(path, &result) == -1) {
        return false;
    }

    if (S_ISDIR(result.st_mode)) {
        return true;
    }
#else
    DWORD attr = GetFileAttributes(path);
    if (attr == INVALID_FILE_ATTRIBUTES) {
        return false;
    }

    if (attr & FILE_ATTRIBUTE_DIRECTORY) {
        return true;
    }
#endif

    return false;
}
