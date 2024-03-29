﻿/*
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

#include <regex>
#include <fstream>
#include <Shlwapi.h>
#include <io.h>

#include "MetatagEX.h"
#include "opensource/hwpx-owpml-model/OWPMLUtil/HncCtrlChDef.h"

#define F_OK 0

bool MetaTagCompare(MetatagEX::METATAG& tag1, MetatagEX::METATAG& tag2) {
    if (tag1.tagName.compare(tag2.tagName) > 0)
        return false;
    return true;
}

OWPML::COwpmlDocumnet* MetatagEX::ExtractMetatag(std::string inputPath, std::string file, std::string regexStr, std::vector<METATAG>* metatag, bool bHeaderOnly, bool isExtract)
{
    std::regex reg;
    try {
        if (regexStr.find(".hwpx") == std::string::npos)
            regexStr.append(".hwpx");

        reg = std::regex(regexStr);
    }
    catch (std::regex_error::exception e) {
        reg = std::regex("^.*.hwpx$");
    }

    std::smatch match;
    if (std::regex_search(file, match, reg) == false) {
        return false;
    }

    std::string srcPath = inputPath + StringResource::PathSeperator + file;
    std::wstring docPath = Util::string_to_wstring(srcPath);

    OWPML::COwpmlDocumnet* document = OWPML::COwpmlDocumnet::OpenDocument(docPath.c_str());
    if (document == NULL) {
        return NULL;

    }

    TraverseHeader(isExtract, srcPath, document, metatag);
    if (bHeaderOnly == false) {
        TraverseSection(isExtract, srcPath, document, metatag);
    }

    return document;
}

void MetatagEX::ExtractMetatag(std::string inputPath, std::string outputPath, Option option, Option dsc, bool bHeaderOnly)
{
    std::vector<std::string> files;
    std::string regexStr = Util::getdir(inputPath, files);

    for (auto& iter_file : files) {
        std::vector<METATAG> metatag;

        OWPML::COwpmlDocumnet* document = ExtractMetatag(inputPath, iter_file, regexStr, &metatag, bHeaderOnly, true);
        if (document == NULL) {
            continue;
        } else {
            if (dsc == Option::Descend)
                std::sort(metatag.begin(), metatag.end());
            else if (dsc == Option::Ascend)
                std::sort(metatag.begin(), metatag.end(), MetaTagCompare);

            tagContainer.push_back(std::make_pair(iter_file, metatag));
        }

        delete document;
    }

    ExportMetatagData(option, outputPath);

}

void MetatagEX::ClaasifyMetatag(std::string inputPath, std::string jsonPath, std::string outputPath, Option option, bool bHeaderOnly)
{
    rapidjson::Document jsonDoc;

    if (!ImportMetatagFromJson(jsonPath, jsonDoc))
        return;

    std::vector<std::string> files;
    std::string regexStr = Util::getdir(inputPath, files);

    for (auto& iter_file : files) {
        std::vector<METATAG> metatag;

        OWPML::COwpmlDocumnet* document = ExtractMetatag(inputPath, iter_file, regexStr, &metatag, bHeaderOnly, false);
        if (document == NULL) {
            continue;
        } else {
            delete document;
        }
    }

    if (option == Option::Console) {
        for (auto& iter_sorted : sortedMetatag) {
            std::cout << iter_sorted.first << " : " << iter_sorted.second << std::endl;
        }
    } else if (option == Option::File) {
        ExportToJson(ExportType::ClaasifyTag, &outputPath);
    }
}

void MetatagEX::ExportToJson(ExportType type, std::string* jsonPath, std::string* srcPath, std::string* oldTagName, std::string* newTagName, METATAG* tag) {
    rapidjson::Document jsonDoc;
    jsonDoc.SetObject();
    auto& allocator = jsonDoc.GetAllocator();

    if (type == ExportType::ExtractTag) {
        for (auto& file : tagContainer) {
            rapidjson::Value fileNameArray(rapidjson::kArrayType);
            fileNameArray.SetArray();
            for (auto& iter : file.second) {
                rapidjson::Value objectAttribute(rapidjson::kObjectType);
                objectAttribute.SetObject();
                rapidjson::Value keyTag("tagName", allocator);
                METATAG tag = iter;

                rapidjson::Value valTag(tag.tagName.c_str(), allocator);
                objectAttribute.AddMember(keyTag, valTag, allocator);

                rapidjson::Value keyPath("filePath", allocator);
                rapidjson::Value valPath(tag.filePath.c_str(), allocator);
                objectAttribute.AddMember(keyPath, valPath, allocator);

                rapidjson::Value keyObj("objectType", allocator);
                rapidjson::Value valObj(tag.objectType.c_str(), allocator);
                objectAttribute.AddMember(keyObj, valObj, allocator);

                std::string contentData = tag.contentText;
                if (!contentData.empty()) {
                    rapidjson::Value keyObjData("contextText", allocator);
                    size_t previous = 0, current = 0;
                    current = contentData.find((char16_t)HWPCH_LINE_BREAK);

                    if (current != hncstd::string::npos) {
                        rapidjson::Value arrayRes(rapidjson::kArrayType);

                        while (current != hncstd::string::npos) {
                            std::string subString = contentData.substr(previous, current - previous);
                            rapidjson::Value valSubString(subString.c_str(), allocator);

                            arrayRes.PushBack(valSubString, allocator);
                            previous = current + 1;
                            current = contentData.find((char16_t)HWPCH_LINE_BREAK, previous);
                            if (previous < contentData.size() && current == hncstd::string::npos) {
                                rapidjson::Value last(contentData.substr(previous, hncstd::string::npos).c_str(), allocator);
                                arrayRes.PushBack(last, allocator);
                            }
                        }
                        objectAttribute.AddMember(keyObjData, arrayRes, allocator);
                    } else {
                        rapidjson::Value valObj(tag.contentText.c_str(), allocator);
                        objectAttribute.AddMember(keyObjData, valObj, allocator);
                    }
                }

                fileNameArray.PushBack(objectAttribute, allocator);
            }
            rapidjson::Value fileName(file.first.c_str(), allocator);
            jsonDoc.AddMember(fileName, fileNameArray, allocator);
        }

    } else if (type == ExportType::ClaasifyTag) {
        std::sort(sortedMetatag.begin(), sortedMetatag.end(), std::less<std::pair<std::string, std::string>>());

        for (auto& iter_sorted : sortedMetatag) {
            rapidjson::Value key(iter_sorted.first.c_str(), allocator);
            rapidjson::Value val(iter_sorted.second.c_str(), allocator);
            jsonDoc.AddMember(key, val, allocator);
        }

    } else if (type == ExportType::ChangeTag) {
        rapidjson::Value fileNameArray(rapidjson::kArrayType);
        fileNameArray.SetArray();

        rapidjson::Value objectAttribute(rapidjson::kObjectType);
        objectAttribute.SetObject();

        rapidjson::Value keyOldName("oldTagName", allocator);
        rapidjson::Value valOldName((*oldTagName).c_str(), allocator);
        objectAttribute.AddMember(keyOldName, valOldName, allocator);

        rapidjson::Value keyNewName("newTagName", allocator);
        rapidjson::Value valNewName((*newTagName).c_str(), allocator);
        objectAttribute.AddMember(keyNewName, valNewName, allocator);

        rapidjson::Value keyObj("objectType", allocator);
        rapidjson::Value valObj(tag->objectType.c_str(), allocator);
        objectAttribute.AddMember(keyObj, valObj, allocator);

        fileNameArray.PushBack(objectAttribute, allocator);

        rapidjson::Value fileName((*srcPath).c_str(), allocator);
        jsonDoc.AddMember(fileName, fileNameArray, allocator);
    }

    std::ofstream ofs((*jsonPath));
    rapidjson::OStreamWrapper osw(ofs);

    rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(osw);
    jsonDoc.Accept(writer);
    ofs.close();
}

bool MetatagEX::ChangeMetatagName(std::string inputPath, std::string jsonPath, std::string oldTagName, std::string newTagName, bool bHeaderOnly, bool bSaveToOrigin/* = false*/)
{
    std::vector<std::string> files;
    std::string regexStr = Util::getdir(inputPath, files);

    std::string srcPath;
    std::wstring docPath;

    for (auto& iter_file : files) {
        std::vector<METATAG> metatag;
        OWPML::COwpmlDocumnet* document = ExtractMetatag(inputPath, iter_file, regexStr, &metatag, bHeaderOnly, true);

        if (document == NULL) {
            continue;
        } else {
            srcPath = inputPath + StringResource::PathSeperator + iter_file;
            docPath = Util::string_to_wstring(srcPath);

            for (auto& tag : metatag) {
                if (SetMetaTagName(tag.object, Util::string_to_utf16(oldTagName), Util::string_to_utf16(newTagName))) {
                    if (jsonPath.empty() == false) {
                        ExportToJson(ExportType::ChangeTag, &jsonPath, &srcPath, &oldTagName, &newTagName, &tag);
                    } else {

                    }
                }
            }
        }

        if (!bSaveToOrigin) {
            WCHAR tmpPath[_MAX_PATH];
            WCHAR index[10] = { 0, };

            wcscpy_s(tmpPath, _countof(tmpPath), docPath.c_str());
            int n = 1;

            while (PathFileExistsW(tmpPath)) {
                ZeroMemory(tmpPath, _MAX_PATH);
                wcscpy_s(tmpPath, _countof(tmpPath), docPath.c_str());
                PathRemoveExtensionW(tmpPath);
                swprintf_s(index, _countof(index), L"_%d", n++);
                wcscat_s(tmpPath, _countof(tmpPath), index);
                PathAddExtensionW(tmpPath, L".hwpx");
            }

            docPath = tmpPath;
        }
        document->SaveDocument(docPath.c_str());

        delete document;
    }

    return true;
}

bool MetatagEX::ImportMetatagFromJson(std::string path, rapidjson::Document &jsonDoc)
{
    if (_access(path.c_str(), F_OK) < 0) {
        std::cout << StringResource::NoJsonFile << std::endl;
        return false;
    }
    std::ifstream ifs(path);
    rapidjson::IStreamWrapper isw(ifs);
    jsonDoc.ParseStream(isw);
    rapidjson::StringBuffer buffer{};
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer{ buffer };
    jsonDoc.Accept(writer);

    if (jsonDoc.HasParseError()) {
        std::cout << "Invalid Input" << std::endl;

        return false;
    }

    rapidjson::Value::MemberIterator itr = jsonDoc.MemberBegin();
    for (itr; itr != jsonDoc.MemberEnd(); ++itr) {
        if (itr->value.GetType() == rapidjson::kArrayType) {

            for (auto& arr : itr->value.GetArray()) {
                if (arr.IsObject()) {
                    rapidjson::Value var = arr.GetObj();
                    tagNameContainer.push_back(var["tagName"].GetString());
                }
            }
        } else if (itr->value.GetType() == rapidjson::kStringType) {
            tagNameContainer.push_back(itr->value.GetString());
        }
    }
    return true;
}

void MetatagEX::ExportMetatagData(Option option, std::string outputPath) {
    if (option == Option::Console) {
        std::vector<std::pair<std::u16string, std::map<std::string, std::string>>>::iterator iter;

        for (auto& iter : tagContainer) {
            std::cout << "fileName : [" << iter.first << std::endl;
            for (auto& file : iter.second) {
                std::cout << "tagName : " << file.tagName << std::endl;

                std::cout << "filePath : " << file.filePath << std::endl;

                std::cout << "objectType : " << file.objectType << std::endl;

                if (file.contentText.empty() == false)
                    std::cout << "contentText : " << file.contentText << std::endl;

                std::cout << std::endl;
            }
            std::cout << "]" << iter.first << std::endl << std::endl;
        }
    } else if (option == Option::File) {
        ExportToJson(ExportType::ExtractTag, &outputPath);
    }
}

void MetatagEX::TraverseHeader(bool isExtract, std::string srcfilePath, OWPML::COwpmlDocumnet* document, std::vector<METATAG>* metatag)
{
    OWPML::Objectlist* objList = document->GetHead()->GetObjectList();
    for (auto& iter_obj : *objList) {
        OWPML::Objectlist result;
        GetMetaTagObject(iter_obj, result);
        for (auto& iterTagObj : result) {
            if (isExtract) {
                ExtractString(srcfilePath, iterTagObj, metatag);
            } else {
                std::string metatagWStr = Util::utf16_to_string(GetMetaTagName(iterTagObj));
                SearchString(srcfilePath, metatagWStr);
            }
        }
    }
}

void MetatagEX::TraverseSection(bool isExtract, std::string srcfilePath, OWPML::COwpmlDocumnet* document, std::vector<METATAG>* metatag)
{
    std::vector<OWPML::CSectionType*>::iterator iter_section = document->GetSections()->begin();
    for (iter_section; iter_section != document->GetSections()->end(); ++iter_section) {
        std::multimap< std::u16string, OWPML::CObject*> objectList;
        GetMetaTagObject(*iter_section, objectList);

        for (auto& iter_object : objectList) {
            if (isExtract) {
                ExtractString(srcfilePath, (iter_object).second, metatag);
            } else {
                std::string metatagWStr = Util::utf16_to_string((iter_object).first);
                SearchString(srcfilePath, metatagWStr);
            }
        }
    }
}

void MetatagEX::ExtractString(std::string srcfilePath, OWPML::CObject* object, std::vector<METATAG>* metatag)
{
    METATAG newTag;
    char fullPath[MAX_PATH];
    char* fileName;
    GetFullPathNameA(srcfilePath.c_str(), MAX_PATH, fullPath, &fileName);

    unsigned int objectId = object->GetParentObj()->GetID();;

    newTag.fileName = fileName;
    newTag.filePath = fullPath;
    newTag.tagName = Util::utf16_to_string(GetMetaTagName(object));
    newTag.objectType = Util::utf16_to_string(GetObjectTypeText(objectId));
    newTag.contentText = Util::utf16_to_string(GetMetaTagContent(object));
    newTag.object = object;

    metatag->push_back(newTag);
}

void MetatagEX::SearchString(std::string srcfilePath, std::string tag) {
    std::vector<std::string>::iterator iter_find = std::find(tagNameContainer.begin(), tagNameContainer.end(), tag);

    if (iter_find != tagNameContainer.end()) {
        char fullPath[MAX_PATH];
        char* fileName;
        GetFullPathNameA(srcfilePath.c_str(), MAX_PATH, fullPath, &fileName);

        sortedMetatag.push_back(make_pair(*iter_find, fullPath));
    }
}

std::u16string MetatagEX::GetObjectTypeText(unsigned int id) {
    std::u16string text;
    switch (id) {
    case ID_PARA_PictureType: // 그림
        text.assign(u"Picture");
        break;
    case ID_PARA_LineType:
        text.assign(u"Line");
        break;
    case ID_PARA_RectangleType:
        text.assign(u"Rectangle");
        break;
    case ID_PARA_EllipseType:
        text.assign(u"Ellipse");
        break;
    case ID_PARA_ArcType:
        text.assign(u"Arc");
        break;
    case ID_PARA_PolygonType:
        text.assign(u"Polygon");
        break;
    case ID_PARA_CurveType:
        text.assign(u"Curve");
        break;
    case ID_PARA_FieldBegin: //누름틀, 상호참조, 자료연결, 하이퍼링크
        text.assign(u"Link");
        break;
    case ID_PARA_Tc: // 셀에 태그
        text.assign(u"Cell in Table");
        break;
    case ID_PARA_TableType: // 표
        text.assign(u"Table");
        break;
    case ID_HEAD_HWPMLHeadType: // 헤더(문서)
        text.assign(u"Document");
        break;
    case ID_PARA_ContainerType:
        text.assign(u"Container");
        break;
    default:
        text.assign(u"");
        assert("새로 추가해야함");
        break;
    }
    return text;
}

bool MetatagEX::SetMetaTagName(OWPML::CObject* object, std::u16string srcTagName, std::u16string destTagName)
{
    std::u16string name;
    if (object->GetID() == ID_PARA_METATAG) {
        name = Util::wstring_to_utf16(((OWPML::CMetaTag*)object)->Getval());
    } else {
        OWPML::CParaListType* paraListType = (OWPML::CParaListType*)object;
        if (paraListType) {
            name = Util::wstring_to_utf16(paraListType->GetMetaTag());
        }
    }

    int firstSharp = name.find(u"#");
    std::u16string subString = name.substr(firstSharp);
    int nPos = subString.find(u"#");
    if (nPos != -1) {
        std::u16string tagName = subString.substr(nPos, subString.rfind(u"\"") - nPos);
        if (tagName.empty() == false) {
            if (tagName.compare(srcTagName) == 0) {
                tagName.append(u"\"");
                destTagName.append(u"\"}");
                name.replace(firstSharp, std::u16string::npos, destTagName);
            } else {
                return false;
            }
        }
    }

    if (object->GetID() == ID_PARA_METATAG) {
        ((OWPML::CMetaTag*)object)->Setval(Util::utf16_to_wstring(name).c_str());
    } else {
        OWPML::CParaListType* paraListType = (OWPML::CParaListType*)object;
        if (paraListType) {
            paraListType->SetMetaTag(Util::utf16_to_wstring(name).c_str());
        }
    }

    return true;
}

std::u16string MetatagEX::GetMetaTagName(OWPML::CObject* object)
{
    std::u16string name;
    if (object->GetID() == ID_PARA_METATAG) {
        name = Util::wstring_to_utf16(((OWPML::CMetaTag*)object)->Getval());
    } else {
        OWPML::CParaListType* paraListType = (OWPML::CParaListType*)object;
        if (paraListType) {
            name = Util::wstring_to_utf16(paraListType->GetMetaTag());
        }
    }

    size_t n = name.find(u"#");
    name = name.substr(n, name.rfind(u"\"") - n);

    return name;
}

void MetatagEX::GetMetaTagObject(OWPML::CObject* searchObject, OWPML::Objectlist& result)
{
    if (searchObject->GetID() == ID_PARA_METATAG) {
        result.push_back(searchObject);
    }

    OWPML::Objectlist* childObjects = searchObject->GetObjectList();
    if (childObjects == NULL) {
        return;
    }

    if (searchObject->GetID() == ID_PARA_ParaListType) {
        std::u16string metaTag = Util::wstring_to_utf16(((OWPML::CParaListType*)searchObject)->GetMetaTag());

        if (metaTag.size() > 0) {
            result.push_back(searchObject);
        }
    }

    std::vector<OWPML::CObject*>::iterator iter_object = childObjects->begin();
    for (iter_object; iter_object != childObjects->end(); iter_object++) {
        GetMetaTagObject(*iter_object, result);
    }
}

void MetatagEX::GetMetaTagObject(OWPML::CObject* searchObject, std::multimap<std::u16string, OWPML::CObject*>& result)
{
    if (searchObject->GetID() == ID_PARA_METATAG) {
        result.insert(std::pair<std::u16string, OWPML::CObject*>(GetMetaTagName(searchObject), searchObject));
    }

    OWPML::Objectlist* childObjects = searchObject->GetObjectList();
    if (childObjects == NULL) {
        return;
    }

    if (searchObject->GetID() == ID_PARA_ParaListType) {
        std::u16string metaTag = Util::wstring_to_utf16(((OWPML::CParaListType*)searchObject)->GetMetaTag());
        if (metaTag.size() > 0) {
            result.insert(std::pair<std::u16string, OWPML::CObject*>(GetMetaTagName(searchObject), searchObject));
        }
    }

    std::vector<OWPML::CObject*>::iterator iter_object = childObjects->begin();
    for (iter_object; iter_object != childObjects->end(); iter_object++) {
        GetMetaTagObject(*iter_object, result);
    }
}

std::u16string MetatagEX::GetMetaTagContent(OWPML::CObject* object)
{
    if (object == NULL) {
        return u"";
    }

    OWPML::CObject* parent = object->GetParentObj();
    OWPML::CPType* para = NULL;
    OWPML::CRunType* run = NULL;
    switch (parent->GetID()) {
    case ID_PARA_FieldBegin:
    {
        OWPML::CFieldBegin* field = NULL;
        field = (OWPML::CFieldBegin*)parent;

        OWPML::FIELDTYPE type = field->GetType();

        while (parent->GetID() != ID_PARA_PType) {
            parent = parent->GetParentObj();
        }

        return ProcessingTextElement(parent, type);
        break;
    }
    case ID_PARA_Tc:
    case ID_PARA_RectangleType:
    case ID_PARA_EllipseType:
    case ID_PARA_ArcType:
    case ID_PARA_PolygonType:
    case ID_PARA_CurveType:
        return ProcessingTextElement(parent);
    default:
        break;
    }

    return u"";
}

/// 메타태그 텍스트 추출
std::u16string MetatagEX::ProcessingTextElement(OWPML::CObject* object, int type)
{
    if (object == NULL) {
        return u"";
    }

    std::u16string text, buff;

    OWPML::Objectlist* pChildList = object->GetObjectList();
    OWPML::CObject* pObject = NULL;

    if (pChildList) { // (#PCDATA | TITLEMARK | TAB | LINEBREAK | HYPEN | NBSPACE | FWSPACE)*

        for (auto pObject : *pChildList) {
            if (pObject) {
                switch (pObject->GetID()) {
                case ID_PARA_ParaListType:
                {
                    text += ProcessingTextElement(pObject);
                    break;
                }
                case ID_PARA_PType:
                {
                    text += ProcessingTextElement(pObject);
                    break;
                }
                case ID_PARA_RunType:
                {
                    OWPML::CRunType* run = (OWPML::CRunType*)pObject;

                    if (run) {
                        if (type == OWPML::FIELDTYPE::FT_CLICK_HERE && run->GetCharPrIDRef() != 0)
                            continue;

                        OWPML::CT* t = run->Gett(0);
                        if (t) {
                            text += ProcessingTextElement(t);
                        }
                    }

                    break;
                }
                case ID_PARA_LineSegArray:
                {
                    text += ((char16_t)HWPCH_LINE_BREAK);
                    break;
                }
                case ID_PARA_Char:
                {
                    buff.clear();
                    OWPML::CChar* pChar = (OWPML::CChar*)pObject;
                    std::u16string valueText = Util::wstring_to_utf16(pChar->Getval());

                    for (int i = 0; i < (int)valueText.size(); i++) {
                        if (valueText.at(i) != HWPCH_LINE_BREAK && valueText.at(i) != HWPCH_PARA_BREAK && valueText.at(i) != HWPCH_TAB)
                            buff += valueText.at(i);
                    }
                    text += buff;
                    break;
                }
                case ID_PARA_DrawText: {
                    text += ProcessingTextElement(pObject);
                    break;
                }
                case ID_PARA_TitleMark:
                {
                    break;
                }
                case ID_PARA_Tab:
                {
                    text += ((char16_t)HWPCH_TAB);
                    break;
                }
                case ID_PARA_LineBreak:
                {
                    text += ((char16_t)HWPCH_LINE_BREAK);
                    break;
                }
                case ID_PARA_Hypen:
                {
                    text += ((char16_t)HWPCH_HYPHEN);
                    break;
                }
                case ID_PARA_FwSpace:
                {
                    text += ((char16_t)HWPCH_FIXED_WIDTH_SPACE);
                    break;
                }
                case ID_PARA_NbSpace:
                {
                    text += ((char16_t)HWPCH_NON_BREAKING_SPACE);
                    break;
                }
                case ID_PARA_MarkpenBegin:
                {
                    break;
                }
                case ID_PARA_MarkpenEnd:
                {
                    break;
                }
                default:
                    break;
                }
            }
        } // for
    }
    return text;
}

OWPML::CPType* MetatagEX::ConvertCelltoPara(OWPML::CTc* tc)
{
    if (tc == NULL || tc->GetID() != ID_PARA_Tc) {
        return NULL;
    }

    OWPML::CTc* newTc = (OWPML::CTc*)(tc->Clone());
    OWPML::CCellAddr* cellAddr = (OWPML::CCellAddr*)(tc->GetObjectByID(ID_PARA_CellAddr));
    cellAddr->SetColAddr(0);
    cellAddr->SetRowAddr(0);

    OWPML::CPType* pPara = OWPML::CPType::Create();
    pPara->SetParaPrIDRef(0);
    pPara->SetStyleIDRef(0);

    OWPML::CRunType* pRun = OWPML::CRunType::Create();

    OWPML::CTableType* pTable = OWPML::CTableType::Create();
    pTable->SetRowCnt(1);
    pTable->SetColCnt(1);

    OWPML::CTr* pTr = OWPML::CTr::Create();

    pTr->SetObject(newTc);
    pTable->SetObject(pTr);
    pRun->SetObject(pTable);
    pPara->SetObject(pRun);

    return pPara;
}