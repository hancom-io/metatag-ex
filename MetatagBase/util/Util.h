/*
 * Copyright 2022 Hancom Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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