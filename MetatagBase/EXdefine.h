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

#ifndef _METATAGEX_DEFINE
#define _METATAGEX_DEFINE

#include <iostream>
namespace StringResource
{
    const std::string InvalidInput = "Invalid Input";
    const std::string NoJsonFile = "Input Json File Not Exists";
    const std::string ExtractionFailed = " Extraction Failed!";
    const std::string Complete = "Complete!";
    const std::string PathSeperator = "\\";
};

namespace Defines
{
    const std::string NODE_HEAD = "hh:head";
    const std::string NODE_HMETATAG = "hh:metaTag";
    const std::string NODE_PMETATAG = "hp:metaTag";
    const std::string NODE_SECTION = "hs:sec";
    const std::string NODE_PARA = "hp:p";
    const std::string NODE_TABLE = "hp:run/hp:tbl";
    const std::string NODE_SUBLIST = "hp:tr/hp:tc/hp:subList";
    const std::string NODE_RECT = "hp:run/hp:rect";
    const std::string NODE_PICTURE = "hp:run/hp:pic";
    const std::string NODE_ELLIPSE = "hp:run/hp:ellipse";
    const std::string NODE_LINE = "hp:run/hp:line";
    const std::string NODE_ARC = "hp:run/hp:arc";
    const std::string NODE_POLYGON = "hp:run/hp:polygon";
    const std::string NODE_CURV = "hp:run/hp:curv";
    const std::string NODE_CONNECTLINE = "hp:run/hp:connectLine";
};

enum Option
{
    File,
    Console,
    Descend,
    Ascend,
    Source,
    Destination,
};

#endif _METATAGEX_DEFINE//