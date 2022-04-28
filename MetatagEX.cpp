#include "MetatagEX.h"
#include <regex>
#ifdef OS_UNIX
#include <dirent.h>
#endif // OS_UNIX
#include <fstream>

#ifndef OS_UNIX
#include "./opensource/hwpx-owpml-model/OWPMLUtil/HncCtrlChDef.h"
#endif

void MetatagEX::Initialize()
{
    MetatagEX::GetSortedMetatag()->clear();
    MetatagEX::GetMetatagContainer()->clear();
    MetatagEX::GetFindMetatagVector()->clear();
    MetatagEX::GetTagIndex(false, true);
    MetatagEX::GetFileIndex(false, true);
}

void MetatagEX::CalcPercentProc(int nFileCnt)
{
    int curIndex = -1;
    while (GetFileIndex(false, false) != nFileCnt)
    {
        if (curIndex != GetFileIndex(false, false))
        {
            curIndex = GetFileIndex(false, false);
            printf("%d", (curIndex * 100) / nFileCnt);
            std::cout << "%" << std::endl;
        }
        curIndex = GetFileIndex(false, false);
    }
    std::cout << StringResource::Complete << std::endl;
    return;
}

bool MetatagEX::ImportMetatagFromJson(std::string path, rapidjson::Document &jsonDoc)
{
    if (Util::IsExistFile(path) == false)
    {
        std::cout << StringResource::NoJsonFile << std::endl;
        return false;
    }
    std::ifstream ifs(path);
    rapidjson::IStreamWrapper isw(ifs);
    jsonDoc.ParseStream(isw);
    rapidjson::StringBuffer buffer{};
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer{ buffer };
    jsonDoc.Accept(writer);

    if (jsonDoc.HasParseError())
    {
        std::cout << "Invalid Input" << std::endl;

        return false;
    }

    rapidjson::Value::MemberIterator itr = jsonDoc.MemberBegin();
    for (itr; itr != jsonDoc.MemberEnd(); ++itr)
    {
        if (itr->value.GetType() == rapidjson::kArrayType) {

            for (auto& arr : itr->value.GetArray()) {
                if (arr.IsObject()) {
                    rapidjson::Value var = arr.GetObj();
                    MetatagEX::GetFindMetatagVector()->push_back(Util::string_to_utf16(var["tag"].GetString()));
                }
            }
        } else if (itr->value.GetType() == rapidjson::kStringType) {
            MetatagEX::GetFindMetatagVector()->push_back(Util::string_to_utf16(itr->value.GetString()));
        }
    }

    return true;
}

void MetatagEX::SortMetatag(std::string inputPath, std::string jsonPath, std::string outputPath, Option option, bool bShowProgress, bool bHeaderOnly)
{
    Initialize();

    rapidjson::Document jsonDoc;

    if (!ImportMetatagFromJson(jsonPath, jsonDoc))
        return;

    std::vector<std::string> files;
    std::string regexStr = Util::getdir(inputPath, files);
    std::vector<std::string>::iterator iter_file = files.begin();
    std::thread* CountingThread = NULL;
    if (bShowProgress)
    {
        CountingThread = new std::thread(CalcPercentProc, files.size());
    }
    for (iter_file; iter_file != files.end(); ++iter_file)
    {
        std::regex reg;
        try
        {
            if (regexStr.find(".hwpx") == std::string::npos)
                regexStr.append(".hwpx");

            reg = std::regex(regexStr);
        }
        catch (std::regex_error::exception e)
        {
            reg = std::regex("^.*.hwpx$");
        }

        MetatagEX::GetFileIndex(true, false);

        std::smatch match;
        if (std::regex_search(*iter_file, match, reg) == false)
        {
            continue;
        }

#ifdef OS_UNIX
        std::string unzipPath = std::string(inputPath + StringResource::PathSeperator + "UnzipTemp");
        if (Util::isDirectory(unzipPath.c_str()))
        {
            Util::removeDirectory(unzipPath.c_str());
            Util::createDirectory(Util::string_to_utf16(unzipPath.c_str()));
        } else
        {
            Util::createDirectory(Util::string_to_utf16(unzipPath.c_str()));
        }

        char path[PATH_MAX];

        sprintf_s(path, "%s", std::string(inputPath + StringResource::PathSeperator + *iter_file).c_str());
        if (Util::extract(path, unzipPath.c_str()) == -1)
        {
            std::cout << std::string(path) + StringResource::PathSeperator + *iter_file + StringResource::ExtractionFailed << std::endl;
            return;
        }

        SearchHeader(unzipPath, inputPath + StringResource::PathSeperator + *iter_file);
        if (bHeaderOnly == false)
        {
            SearchSection(unzipPath, inputPath + StringResource::PathSeperator + *iter_file);
        }
        Util::removeDirectory(unzipPath.c_str());

#else
        std::string srcPath = inputPath + StringResource::PathSeperator + *iter_file;
        std::wstring docPath = Util::string_to_wstring(srcPath);

        OWPML::COwpmlDocumnet* document = OWPML::COwpmlDocumnet::OpenDocument(docPath.c_str());
        if (document == NULL) {
            return;

        }

        SearchHeader("", srcPath, document);
        if (bHeaderOnly == false)
        {
            SearchSection("", srcPath, document);
        }
#endif

    }
    if (CountingThread != NULL)
    {
        CountingThread->join();
        delete CountingThread;
    }
    if (option == Option::Console)
    {
        std::vector<std::pair<std::string, std::u16string>>::iterator iter_sorted = MetatagEX::GetSortedMetatag()->begin();
        for (iter_sorted; iter_sorted != MetatagEX::GetSortedMetatag()->end(); ++iter_sorted)
        {
            std::cout << iter_sorted->first << " : " << Util::utf16_to_string(iter_sorted->second) << std::endl;
        }
    } else if (option == Option::File)
    {
        std::vector<std::pair<std::string, std::u16string>>::iterator iter_sorted = MetatagEX::GetSortedMetatag()->begin();
        jsonDoc.SetObject();
        for (iter_sorted; iter_sorted != MetatagEX::GetSortedMetatag()->end(); ++iter_sorted)
        {
            auto& allocator = jsonDoc.GetAllocator();
            rapidjson::Value key(iter_sorted->first.c_str(), allocator);
            rapidjson::Value val(Util::utf16_to_string(iter_sorted->second).c_str(), allocator);
            jsonDoc.AddMember(key, val, allocator);
        }
        std::ofstream ofs(outputPath);
        rapidjson::OStreamWrapper osw(ofs);

        rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(osw);
        jsonDoc.Accept(writer);
        ofs.close();
    }
}


void MetatagEX::SearchHeader(std::string path, std::string srcfilePath, OWPML::COwpmlDocumnet* document)
{
#ifdef OS_UNIX
    XmlParser headerParser;
    headerParser.readFile(std::string(path + StringResource::PathSeperator + "Contents" + StringResource::PathSeperator + "header.xml").c_str());
    DOMNode* pNodeHeader = XmlParser::SelectSingleNode(Defines::NODE_HEAD, headerParser.GetRootElement());
    std::list<DOMNode*> docMetatagList;
    headerParser.SelectNodes(Defines::NODE_HMETATAG, pNodeHeader, &docMetatagList);
    std::list<DOMNode*>::iterator iter = docMetatagList.begin();
    for (iter; iter != docMetatagList.end(); ++iter)
    {
        SearchString(srcfilePath, *iter);
    }
#else
    OWPML::Objectlist* objList = document->GetHead()->GetObjectList();
    OWPML::Objectlist::iterator iter_obj = objList->begin();
    for (iter_obj; iter_obj != objList->end(); ++iter_obj)
    {
        OWPML::Objectlist result;
        GetMetaTagObject(*iter_obj, result);
        OWPML::Objectlist::iterator iterTagObj = result.begin();
        for (iterTagObj; iterTagObj != result.end(); ++iterTagObj)
        {
            std::u16string metatagWStr = GetMetaTagName(*iterTagObj);
            SearchString(srcfilePath, metatagWStr);
        }
    }
#endif
}


void MetatagEX::SearchSection(std::string path, std::string srcfilePath, OWPML::COwpmlDocumnet* document)
{
#ifdef OS_UNIX
    bool isEndOfSection = false;
    int sectionIndex = 0;
    while (isEndOfSection == false) {
        std::string sectionFilePath = path + StringResource::PathSeperator + "Contents";
        char filename[50] = { 0, };
        snprintf(filename, sizeof(char) * 50, std::string(StringResource::PathSeperator + "section%d.xml").c_str(), sectionIndex);

        sectionFilePath.append(filename);

        // Next file index
        sectionIndex++;

        // Is exist file
        if (Util::IsExistFile(sectionFilePath) == false) {
            isEndOfSection = true;
            continue;
        }

        // ----------------------------------------------
        // Read File
        XmlParser xmlParser;
        xmlParser.readFile(sectionFilePath);

        // Section
        DOMNode* pNodeSection = XmlParser::SelectSingleNode(Defines::NODE_SECTION, xmlParser.GetRootElement());
        if (pNodeSection == NULL) {
            return;
        }

        std::list<DOMNode*> nodelist;
        std::list<DOMNode*>::iterator itr;
        XmlParser::SelectNodes(Defines::NODE_PARA, pNodeSection, &nodelist);

        for (itr = nodelist.begin(); itr != nodelist.end(); ++itr)
        {
            std::list<DOMNode*> tblList;
            XmlParser::SelectNodes(Defines::NODE_TABLE, *itr, &tblList);
            std::list<DOMNode*>::iterator iter_tbl = tblList.begin();
            for (iter_tbl; iter_tbl != tblList.end(); ++iter_tbl)
            {
                SortTable(*iter_tbl, srcfilePath);
            }
            tblList.clear();
            SortShape(*itr, srcfilePath);
        }
    }
#else
    std::vector<OWPML::CSectionType*>::iterator iter_section = document->GetSections()->begin();
    for (iter_section; iter_section != document->GetSections()->end(); ++iter_section)
    {
        std::multimap<std::u16string, OWPML::CObject*> objectList;
        GetMetaTagObject(*iter_section, objectList);
        std::multimap<std::u16string, OWPML::CObject*>::iterator iter_object = objectList.begin();

        for (iter_object; iter_object != objectList.end(); ++iter_object)
        {
            std::u16string metatagWStr = (*iter_object).first;
            SearchString(srcfilePath, metatagWStr);
        }

    }
#endif
}


void MetatagEX::SearchString(std::string srcfilePath, std::u16string tag) {
    std::vector<std::u16string>::iterator iter_find = std::find(MetatagEX::GetFindMetatagVector()->begin(), MetatagEX::GetFindMetatagVector()->end(), tag);

    if (iter_find != MetatagEX::GetFindMetatagVector()->end())
    {

        char fullPath[MAX_PATH];
        char* fileName;
        GetFullPathName(srcfilePath.c_str(), MAX_PATH, fullPath, &fileName);

        MetatagEX::GetSortedMetatag()->push_back(make_pair(fullPath, *iter_find));
    }

}

#ifdef OS_UNIX
void MetatagEX::SearchString(std::string srcfilePath, DOMNode* node)
{
    std::u16string contents;
    contents.assign(reinterpret_cast<const char16_t*>(node->getTextContent()));
    int pos = contents.find(u"#");
    int endpos = contents.find(u",", pos);
    if (endpos == std::string::npos)
        endpos = contents.find(u"\"", pos);
    while (pos != std::string::npos)
    {
        std::u16string token = contents.substr(pos, endpos - pos);

        std::vector<std::u16string>::iterator iter_find = std::find(MetatagEX::GetFindMetatagVector()->begin(), MetatagEX::GetFindMetatagVector()->end(), token);
        if (iter_find != MetatagEX::GetFindMetatagVector()->end())
        {
#ifdef OS_UNIX
            char fullPath[PATH_MAX];
            realpath(srcfilePath.c_str(), fullPath);
#else // OS_UNIX
            char fullPath[MAX_PATH];
            char* fileName;
            GetFullPathName(srcfilePath.c_str(), MAX_PATH, fullPath, &fileName);
#endif
            MetatagEX::GetSortedMetatag()->push_back(make_pair(fullPath, *iter_find));
        }

        pos = contents.find(u"#", endpos);
        endpos = contents.find(u",", pos);
        if (endpos == std::string::npos)
            endpos = contents.find(u"\"", pos);
    }
}


void MetatagEX::SortTable(DOMNode* node, std::string srcfilePath)
{
    std::list<DOMNode*> tblMetatagList;
    std::list<DOMNode*>::iterator iter_tag;
    XmlParser::SelectNodes(Defines::NODE_PMETATAG, node, &tblMetatagList);
    for (iter_tag = tblMetatagList.begin(); iter_tag != tblMetatagList.end(); ++iter_tag)
    {
        SearchString(srcfilePath, *iter_tag);
    }
    std::list<DOMNode*> cellList;
    std::list<DOMNode*>::iterator iter_cell;
    XmlParser::SelectNodes(Defines::NODE_SUBLIST, node, &cellList);
    for (iter_cell = cellList.begin(); iter_cell != cellList.end(); ++iter_cell)
    {
        XMLCh* attritbuteID = XMLString::transcode("metatag");
        DOMNode* attribute = (*iter_cell)->getAttributes()->getNamedItem(attritbuteID);
        if (attribute != NULL)
        {
            std::string metaTagsStr = XmlParser::ConvertToString(attribute->getTextContent());
            int pos = metaTagsStr.find("#");
            int endpos = metaTagsStr.find(",", pos);
            if (endpos == std::string::npos)
                endpos = metaTagsStr.find("\"", pos);
            while (pos != std::string::npos)
            {
                std::string token = metaTagsStr.substr(pos, endpos - pos);

                std::vector<std::u16string>::iterator iter_find = std::find(MetatagEX::GetFindMetatagVector()->begin(), MetatagEX::GetFindMetatagVector()->end(), Util::string_to_utf16(token));
                if (iter_find != MetatagEX::GetFindMetatagVector()->end())
                {
#ifdef OS_UNIX
                    char fullPath[PATH_MAX];
                    realpath(srcfilePath.c_str(), fullPath);
#else // OS_UNIX
                    char fullPath[MAX_PATH];
                    char* fileName;
                    GetFullPathName(srcfilePath.c_str(), MAX_PATH, fullPath, &fileName);
#endif
                    MetatagEX::GetSortedMetatag()->push_back(make_pair(fullPath, *iter_find));
                }

                pos = metaTagsStr.find("#", endpos);
                endpos = metaTagsStr.find(",", pos);
                if (endpos == std::string::npos)
                    endpos = metaTagsStr.find("\"", pos);
            }
        }
        XMLString::release(&attritbuteID);
        std::list<DOMNode*> nodelist;
        std::list<DOMNode*>::iterator itr;
        XmlParser::SelectNodes(Defines::NODE_PARA, *iter_cell, &nodelist);
        for (itr = nodelist.begin(); itr != nodelist.end(); ++itr)
        {
            std::list<DOMNode*> tblList;
            XmlParser::SelectNodes(Defines::NODE_TABLE, *itr, &tblList);
            std::list<DOMNode*>::iterator iter_tbl = tblList.begin();
            for (iter_tbl; iter_tbl != tblList.end(); ++iter_tbl)
            {
                SortTable(*iter_tbl, srcfilePath);
            }
            tblList.clear();
            SortShape(*itr, srcfilePath);
        }
    }
}

void MetatagEX::SortShape(DOMNode* node, std::string srcfilePath)
{
    std::list<DOMNode*> shapeList;
    XmlParser::SelectNodes(Defines::NODE_RECT, node, &shapeList);
    std::list<DOMNode*>::iterator iter_shape = shapeList.begin();
    for (iter_shape; iter_shape != shapeList.end(); ++iter_shape)
    {
        SearchString(srcfilePath, *iter_shape);
    }
    shapeList.clear();
    XmlParser::SelectNodes(Defines::NODE_PICTURE, node, &shapeList);
    for (iter_shape = shapeList.begin(); iter_shape != shapeList.end(); ++iter_shape)
    {
        SearchString(srcfilePath, *iter_shape);
    }
    shapeList.clear();
    XmlParser::SelectNodes(Defines::NODE_ELLIPSE, node, &shapeList);
    for (iter_shape = shapeList.begin(); iter_shape != shapeList.end(); ++iter_shape)
    {
        SearchString(srcfilePath, *iter_shape);
    }
    shapeList.clear();
    XmlParser::SelectNodes(Defines::NODE_LINE, node, &shapeList);
    for (iter_shape = shapeList.begin(); iter_shape != shapeList.end(); ++iter_shape)
    {
        SearchString(srcfilePath, *iter_shape);
    }
    shapeList.clear();
    XmlParser::SelectNodes(Defines::NODE_ARC, node, &shapeList);
    for (iter_shape = shapeList.begin(); iter_shape != shapeList.end(); ++iter_shape)
    {
        SearchString(srcfilePath, *iter_shape);
    }
    shapeList.clear();
    XmlParser::SelectNodes(Defines::NODE_POLYGON, node, &shapeList);
    for (iter_shape = shapeList.begin(); iter_shape != shapeList.end(); ++iter_shape)
    {
        SearchString(srcfilePath, *iter_shape);
    }
    shapeList.clear();
    XmlParser::SelectNodes(Defines::NODE_CURV, node, &shapeList);
    for (iter_shape = shapeList.begin(); iter_shape != shapeList.end(); ++iter_shape)
    {
        SearchString(srcfilePath, *iter_shape);
    }
    shapeList.clear();
    XmlParser::SelectNodes(Defines::NODE_CONNECTLINE, node, &shapeList);
    for (iter_shape = shapeList.begin(); iter_shape != shapeList.end(); ++iter_shape)
    {
        SearchString(srcfilePath, *iter_shape);
    }
    shapeList.clear();
}

#endif
void MetatagEX::ExtractMetatag(std::string inputPath, std::string outputPath, Option option, Option dsc, bool bShowProgress, bool bHeaderOnly)
{
    Initialize();

    rapidjson::Document jsonDoc;
    jsonDoc.SetObject();

    std::vector<std::string> files;
    std::string regexStr = Util::getdir(inputPath, files);
    std::vector<std::string>::iterator iter_file = files.begin();
    std::thread* CountingThread = NULL;
    if (bShowProgress)
    {
        CountingThread = new std::thread(CalcPercentProc, files.size());
    }
    for (iter_file; iter_file != files.end(); ++iter_file)
    {
        std::regex reg;
        try
        {
            if (regexStr.find(".hwpx") == std::string::npos)
                regexStr.append(".hwpx");

            reg = std::regex(regexStr);
        }
        catch (std::regex_error::exception e)
        {
            reg = std::regex("^.*.hwpx$");
        }

        MetatagEX::GetFileIndex(true, false);

        std::smatch match;
        if (std::regex_search(*iter_file, match, reg) == false)
        {
            continue;
        }

#ifdef OS_UNIX
        std::string unzipPath = std::string(inputPath + StringResource::PathSeperator + "UnzipTemp");

        if (Util::isDirectory(unzipPath.c_str()))
        {
            Util::removeDirectory(unzipPath.c_str());
            Util::createDirectory(Util::string_to_utf16(unzipPath.c_str()));
        } else
        {
            Util::createDirectory(Util::string_to_utf16(unzipPath.c_str()));
        }

        char path[PATH_MAX];
        sprintf_s(path, "%s", std::string(inputPath + StringResource::PathSeperator + *iter_file).c_str());
        if (Util::extract(path, unzipPath.c_str()) == -1)
        {
            std::cout << std::string(path) + StringResource::PathSeperator + *iter_file + StringResource::ExtractionFailed << std::endl;
            return;
        }

        TraverseHeader(unzipPath, inputPath + StringResource::PathSeperator + *iter_file);
        if (bHeaderOnly == false)
        {
            TraverseSection(unzipPath, inputPath + StringResource::PathSeperator + *iter_file);
        }
        Util::removeDirectory(unzipPath.c_str());
#else
        std::string srcPath = inputPath + StringResource::PathSeperator + *iter_file;
        std::wstring docPath = Util::string_to_wstring(srcPath);

        OWPML::COwpmlDocumnet* document = OWPML::COwpmlDocumnet::OpenDocument(docPath.c_str());
        if (document == NULL) {
            return;

        }
        TraverseHeader("", srcPath, document);
        if (bHeaderOnly == false)
        {
            TraverseSection("", srcPath, document);
        }

#endif // OS_UNIX


    }

    if (CountingThread != NULL)
    {
        CountingThread->join();
        delete CountingThread;
    }
    // Order
    if (dsc == Option::Descend)
    {
        std::sort(MetatagEX::GetMetatagContainer()->begin(), MetatagEX::GetMetatagContainer()->end(), std::greater<std::pair<std::u16string, std::map<std::string, std::string>>>());
        std::unique(MetatagEX::GetMetatagContainer()->begin(), MetatagEX::GetMetatagContainer()->end());
    } else
    {
        std::sort(MetatagEX::GetMetatagContainer()->begin(), MetatagEX::GetMetatagContainer()->end(), std::less<std::pair<std::u16string, std::map<std::string, std::string>>>());
        std::unique(MetatagEX::GetMetatagContainer()->begin(), MetatagEX::GetMetatagContainer()->end());
    }

    std::vector<std::pair<std::u16string, std::map<std::string, std::string>>>::iterator iter;
    if (option == Option::Console)
    {
        for (iter = MetatagEX::GetMetatagContainer()->begin(); iter != MetatagEX::GetMetatagContainer()->end(); ++iter)
        {
            std::cout << "tag : " << Util::utf16_to_string(iter->first) << std::endl;
            if (iter->second.find("path") != iter->second.end())
            {
                std::cout << "path : " << iter->second["path"] << std::endl;
            }
            if (iter->second.find("object") != iter->second.end())
            {
                std::cout << "object : " << iter->second["object"] << std::endl;
            }
            if (iter->second.find("data") != iter->second.end())
            {
                std::cout << "data : " << iter->second["data"] << std::endl;
            }
            std::cout << std::endl;
        }
    } else if (option == Option::File)
    {
        rapidjson::Value valueArray(rapidjson::kArrayType);
        valueArray.SetArray();
        auto& allocator = jsonDoc.GetAllocator();
        for (iter = MetatagEX::GetMetatagContainer()->begin(); iter != MetatagEX::GetMetatagContainer()->end(); ++iter)
        {
            rapidjson::Value data(rapidjson::kObjectType);
            data.SetObject();
            rapidjson::Value keyTag("tag", allocator);
            rapidjson::Value valTag(Util::utf16_to_string(iter->first).c_str(), allocator);
            data.AddMember(keyTag, valTag, allocator);
            if (iter->second.find("path") != iter->second.end())
            {
                rapidjson::Value keyPath("path", allocator);
                rapidjson::Value valPath(iter->second["path"].c_str(), allocator);
                data.AddMember(keyPath, valPath, allocator);
            }
            if (iter->second.find("object") != iter->second.end())
            {
                rapidjson::Value keyObj("object", allocator);
                rapidjson::Value valObj(iter->second["object"].c_str(), allocator);
                data.AddMember(keyObj, valObj, allocator);
            }
            if (iter->second.find("data") != iter->second.end())
            {
                rapidjson::Value keyObj("data", allocator);
                rapidjson::Value valObj(iter->second["data"].c_str(), allocator);
                data.AddMember(keyObj, valObj, allocator);
            }
            valueArray.PushBack(data, allocator);
        }
        jsonDoc.AddMember("item", valueArray, allocator);
        std::ofstream ofs(outputPath);
        rapidjson::OStreamWrapper osw(ofs);

        rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(osw);
        jsonDoc.Accept(writer);
        ofs.close();
    }
}
void MetatagEX::TraverseHeader(std::string path, std::string srcfilePath, OWPML::COwpmlDocumnet* document)
{
#ifdef OS_UNIX
    XmlParser headerParser;
    headerParser.readFile(std::string(path + StringResource::PathSeperator + "Contents" + StringResource::PathSeperator + "header.xml").c_str());
    DOMNode* pNodeHeader = XmlParser::SelectSingleNode(Defines::NODE_HEAD, headerParser.GetRootElement());
    std::list<DOMNode*> docMetatagList;
    headerParser.SelectNodes(Defines::NODE_HMETATAG, pNodeHeader, &docMetatagList);
    std::list<DOMNode*>::iterator iter = docMetatagList.begin();
    for (iter; iter != docMetatagList.end(); ++iter)
    {
        ExtractString(srcfilePath, *iter, "document");
    }
#else
    OWPML::Objectlist* objList = document->GetHead()->GetObjectList();
    OWPML::Objectlist::iterator iter_obj = objList->begin();
    for (iter_obj; iter_obj != objList->end(); ++iter_obj)
    {
        OWPML::Objectlist result;
        GetMetaTagObject(*iter_obj, result);
        OWPML::Objectlist::iterator iterTagObj = result.begin();
        for (iterTagObj; iterTagObj != result.end(); ++iterTagObj)
        {
            std::u16string metatagWStr = GetMetaTagName(*iterTagObj);
            unsigned int objectId = (*iterTagObj)->GetParentObj()->GetID();;
            std::u16string objectType = GetObjectTypeText(objectId);
            std::string str = Util::utf16_to_string(objectType);

            ExtractString(srcfilePath, metatagWStr, str, "");
        }
    }

#endif


}

void MetatagEX::TraverseSection(std::string path, std::string srcfilePath, OWPML::COwpmlDocumnet* document)
{
#ifdef OS_UNIX
    bool isEndOfSection = false;
    int sectionIndex = 0;
    while (isEndOfSection == false) {

        std::string sectionFilePath = path + StringResource::PathSeperator + "Contents";

        char filename[50] = { 0, };
        snprintf(filename, sizeof(char) * 50, std::string(StringResource::PathSeperator + "section%d.xml").c_str(), sectionIndex);

        sectionFilePath.append(filename);

        // Next file index
        sectionIndex++;

        // Is exist file
        if (Util::IsExistFile(sectionFilePath) == false) {
            isEndOfSection = true;
            continue;
        }

        // ----------------------------------------------
        // Read File
        XmlParser xmlParser;
        xmlParser.readFile(sectionFilePath);

        // Section
        DOMNode* pNodeSection = XmlParser::SelectSingleNode(Defines::NODE_SECTION, xmlParser.GetRootElement());
        if (pNodeSection == NULL) {
            return;
        }

        std::list<DOMNode*> nodelist;
        std::list<DOMNode*>::iterator itr;
        XmlParser::SelectNodes(Defines::NODE_PARA, pNodeSection, &nodelist);

        for (itr = nodelist.begin(); itr != nodelist.end(); ++itr)
        {
            std::list<DOMNode*> tblList;
            XmlParser::SelectNodes(Defines::NODE_TABLE, *itr, &tblList);
            std::list<DOMNode*>::iterator iter_tbl = tblList.begin();
            for (iter_tbl; iter_tbl != tblList.end(); ++iter_tbl)
            {
                TraverseTable(*iter_tbl, srcfilePath);
            }
            tblList.clear();
            TraverseShape(*itr, srcfilePath);
        }
    }
#else
    std::vector<OWPML::CSectionType*>::iterator iter_section = document->GetSections()->begin();
    for (iter_section; iter_section != document->GetSections()->end(); ++iter_section)
    {
        std::multimap< std::u16string, OWPML::CObject*> objectList;
        GetMetaTagObject(*iter_section, objectList);
        std::multimap< std::u16string, OWPML::CObject*>::iterator iter_object = objectList.begin();

        for (iter_object; iter_object != objectList.end(); ++iter_object)
        {
            std::u16string metatagWStr = (*iter_object).first;

            unsigned int objectId = (*iter_object).second->GetParentObj()->GetID();
            std::string objectType = Util::utf16_to_string(GetObjectTypeText(objectId));

            std::u16string contentText = GetMetaTagContent((*iter_object).second);
            std::string text = Util::utf16_to_string(contentText);

            ExtractString(srcfilePath, metatagWStr, objectType, text);

        }

    }
#endif

}

#ifdef OS_UNIX
void MetatagEX::TraverseShape(DOMNode* node, std::string srcfilePath)
{
    std::list<DOMNode*> shapeList;
    XmlParser::SelectNodes(Defines::NODE_RECT, node, &shapeList);
    std::list<DOMNode*>::iterator iter_shape = shapeList.begin();
    for (iter_shape; iter_shape != shapeList.end(); ++iter_shape)
    {
        ExtractShape(*iter_shape, srcfilePath);
    }
    shapeList.clear();
    XmlParser::SelectNodes(Defines::NODE_PICTURE, node, &shapeList);
    for (iter_shape = shapeList.begin(); iter_shape != shapeList.end(); ++iter_shape)
    {
        ExtractShape(*iter_shape, srcfilePath);
    }
    shapeList.clear();
    XmlParser::SelectNodes(Defines::NODE_ELLIPSE, node, &shapeList);
    for (iter_shape = shapeList.begin(); iter_shape != shapeList.end(); ++iter_shape)
    {
        ExtractShape(*iter_shape, srcfilePath);
    }
    shapeList.clear();
    XmlParser::SelectNodes(Defines::NODE_LINE, node, &shapeList);
    for (iter_shape = shapeList.begin(); iter_shape != shapeList.end(); ++iter_shape)
    {
        ExtractShape(*iter_shape, srcfilePath);
    }
    shapeList.clear();
    XmlParser::SelectNodes(Defines::NODE_ARC, node, &shapeList);
    for (iter_shape = shapeList.begin(); iter_shape != shapeList.end(); ++iter_shape)
    {
        ExtractShape(*iter_shape, srcfilePath);
    }
    shapeList.clear();
    XmlParser::SelectNodes(Defines::NODE_POLYGON, node, &shapeList);
    for (iter_shape = shapeList.begin(); iter_shape != shapeList.end(); ++iter_shape)
    {
        ExtractShape(*iter_shape, srcfilePath);
    }
    shapeList.clear();
    XmlParser::SelectNodes(Defines::NODE_CURV, node, &shapeList);
    for (iter_shape = shapeList.begin(); iter_shape != shapeList.end(); ++iter_shape)
    {
        ExtractShape(*iter_shape, srcfilePath);
    }
    shapeList.clear();
    XmlParser::SelectNodes(Defines::NODE_CONNECTLINE, node, &shapeList);
    for (iter_shape = shapeList.begin(); iter_shape != shapeList.end(); ++iter_shape)
    {
        ExtractShape(*iter_shape, srcfilePath);
    }
    shapeList.clear();
}

void MetatagEX::TraverseTable(DOMNode* node, std::string srcfilePath)
{
    std::list<DOMNode*> tblMetatagList;
    std::list<DOMNode*>::iterator iter_tag;
    XmlParser::SelectNodes(Defines::NODE_PMETATAG, node, &tblMetatagList);
    for (iter_tag = tblMetatagList.begin(); iter_tag != tblMetatagList.end(); ++iter_tag)
    {
        ExtractString(srcfilePath, *iter_tag, "table");
    }
    std::list<DOMNode*> cellList;
    std::list<DOMNode*>::iterator iter_cell;
    XmlParser::SelectNodes(Defines::NODE_SUBLIST, node, &cellList);
    for (iter_cell = cellList.begin(); iter_cell != cellList.end(); ++iter_cell)
    {
        XMLCh* attritbuteID = XMLString::transcode("metatag");
        DOMNode* attribute = (*iter_cell)->getAttributes()->getNamedItem(attritbuteID);
        if (attribute != NULL)
        {
            std::string metaTagsStr = XmlParser::ConvertToString(attribute->getTextContent());
            int pos = metaTagsStr.find("#");
            int endpos = metaTagsStr.find(",", pos);
            if (endpos == std::string::npos)
                endpos = metaTagsStr.find("\"", pos);
            while (pos != std::string::npos)
            {
#ifdef OS_UNIX
                char fullPath[PATH_MAX];
                realpath(srcfilePath.c_str(), fullPath);
#else // OS_UNIX
                char fullPath[MAX_PATH];
                char* fileName;
                GetFullPathName(srcfilePath.c_str(), MAX_PATH, fullPath, &fileName);
#endif
                std::string token = metaTagsStr.substr(pos, endpos - pos);
                std::map<std::string, std::string> tempMap;
                tempMap.insert(std::make_pair("path", fullPath));
                tempMap.insert(std::make_pair("object", "table"));
                MetatagEX::GetMetatagContainer()->push_back(std::make_pair(Util::string_to_utf16(token), tempMap));
                pos = metaTagsStr.find("#", endpos);
                endpos = metaTagsStr.find(",", pos);
                if (endpos == std::string::npos)
                    endpos = metaTagsStr.find("\"", pos);
            }
        }
        XMLString::release(&attritbuteID);
        std::list<DOMNode*> nodelist;
        std::list<DOMNode*>::iterator itr;
        XmlParser::SelectNodes(Defines::NODE_PARA, *iter_cell, &nodelist);
        for (itr = nodelist.begin(); itr != nodelist.end(); ++itr)
        {
            std::list<DOMNode*> tblList;
            XmlParser::SelectNodes(Defines::NODE_TABLE, *itr, &tblList);
            std::list<DOMNode*>::iterator iter_tbl = tblList.begin();
            for (iter_tbl; iter_tbl != tblList.end(); ++iter_tbl)
            {
                TraverseTable(*iter_tbl, srcfilePath);
            }
            tblList.clear();
            TraverseShape(*itr, srcfilePath);
        }
    }
}

void MetatagEX::ExtractShape(DOMNode* node, std::string srcfilePath)
{
    std::list<DOMNode*> shapeMetatagList;
    std::list<DOMNode*>::iterator iter_tag;
    XmlParser::SelectNodes(Defines::NODE_PMETATAG, node, &shapeMetatagList);
    for (iter_tag = shapeMetatagList.begin(); iter_tag != shapeMetatagList.end(); ++iter_tag)
    {
        ExtractString(srcfilePath, *iter_tag, "shape");
    }
}

void MetatagEX::ExtractString(std::string srcfilePath, DOMNode* node, std::string origin)
{
    std::u16string contents;
    contents.assign(reinterpret_cast<const char16_t*>(node->getTextContent()));
    int pos = contents.find(u"#");
    int endpos = contents.find(u",", pos);
    if (endpos == std::string::npos)
        endpos = contents.find(u"\"", pos);
    while (pos != std::string::npos)
    {
#ifdef OS_UNIX
        char fullPath[PATH_MAX];
        realpath(srcfilePath.c_str(), fullPath);
#else // OS_UNIX
        char fullPath[MAX_PATH];
        char* fileName;
        GetFullPathName(srcfilePath.c_str(), MAX_PATH, fullPath, &fileName);
#endif
        std::u16string token = contents.substr(pos, endpos - pos);
        std::map<std::string, std::string> tempMap;
        tempMap.insert(std::make_pair("path", fullPath));
        tempMap.insert(std::make_pair("object", origin));
        MetatagEX::GetMetatagContainer()->push_back(std::make_pair(token, tempMap));
        pos = contents.find(u"#", endpos);
        endpos = contents.find(u",", pos);
        if (endpos == std::string::npos)
            endpos = contents.find(u"\"", pos);
    }
}
#else 
void MetatagEX::ExtractString(std::string srcfilePath, std::u16string tag, std::string origin, std::string contentText)
{
    char fullPath[MAX_PATH];
    char* fileName;
    GetFullPathName(srcfilePath.c_str(), MAX_PATH, fullPath, &fileName);

    std::map<std::string, std::string> tempMap;
    tempMap.insert(std::make_pair("path", fullPath));
    tempMap.insert(std::make_pair("object", origin));
    tempMap.insert(std::make_pair("data", contentText));
    MetatagEX::GetMetatagContainer()->push_back(std::make_pair(tag, tempMap));
}
#endif

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

//
//void MetatagEX::SetMetaTagName(OWPML::CObject* object, std::u16string srcTagName, std::u16string destTagName)
//{
//    CHncStringW name;
//    if (object->GetID() == ID_PARA_METATAG) {
//        name = ((OWPML::CMetaTag*)object)->Getval();
//    } else {
//        OWPML::CParaListType* paraListType = (OWPML::CParaListType*)object;
//        if (paraListType) {
//            name = paraListType->GetMetaTag();
//        }
//    }
//
//    CHncStringW midStr = name.Mid(name.Find(L'#'));
//    int nPos = midStr.Find(L'#');
//    while (nPos != -1)
//    {
//        CHncStringW leftStr = midStr.Left(midStr.Find(L','));
//        if (leftStr.IsEmpty() == false)
//        {
//            if (leftStr.Compare(srcTagName) == 0)
//            {
//                leftStr.Concat(L',');
//                destTagName.Concat(L',');
//                name.Replace(leftStr, destTagName);
//                destTagName = destTagName.Left(L',');
//            }
//        } else
//        {
//            leftStr = midStr.Left(midStr.Find(L'\"'));
//            if (leftStr.IsEmpty() == false)
//            {
//                if (leftStr.Compare(srcTagName) == 0)
//                {
//                    leftStr.Concat(L'\"');
//                    destTagName.Concat(L'\"');
//                    name.Replace(leftStr, destTagName);
//                    midStr.Empty();
//                }
//            }
//        }
//        if (midStr.Find(L',') != -1)
//            midStr = midStr.Mid(midStr.Find(L',') + 1);
//        else
//            midStr.Empty();
//        nPos = midStr.Find(L'#');
//    }
//
//    if (object->GetID() == ID_PARA_METATAG) {
//        ((OWPML::CMetaTag*)object)->Setval(name);
//    } else {
//        OWPML::CParaListType* paraListType = (OWPML::CParaListType*)object;
//        if (paraListType) {
//            paraListType->SetMetaTag(name);
//        }
//    }
//}

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
    switch (parent->GetID())
    {
    case ID_PARA_FieldBegin:
    {
        OWPML::CFieldBegin* field = NULL;
        field = (OWPML::CFieldBegin*)parent;

        OWPML::FIELDTYPE type = field->GetType();

        while (parent->GetID() != ID_PARA_PType) {
            parent = parent->GetParentObj();
        }

        para = (OWPML::CPType*)parent;

        int paraCount = para->GetChildCount();
        for (int i = 0; i < paraCount; i++) {
            run = para->Getrun(i);
            if (run) {
                if (type == OWPML::FIELDTYPE::FT_CLICK_HERE && run->GetCharPrIDRef() != 0)
                    continue;

                OWPML::CT* text = run->Gett(0);
                if (text)
                    return ProcessingTextElement(text);
            }
        }
        break;
    }
    case ID_PARA_Tc:
    {
        OWPML::Objectlist* pObjectList = parent->GetObjectList();
        OWPML::Objectlist::iterator iter = pObjectList->begin();
        for (iter; iter != pObjectList->end(); ++iter) {
            OWPML::CObject* pObj = (*iter);
            if (pObj && pObj->GetID() == ID_PARA_ParaListType) {
                OWPML::Objectlist* pSubListChild = pObj->GetObjectList();
                OWPML::Objectlist::iterator subIter = pSubListChild->begin();

                for (subIter; subIter != pSubListChild->end(); ++subIter) {
                    OWPML::CObject* pSubObj = (*subIter);
                    if (pSubObj && pSubObj->GetID() == ID_PARA_PType) {
                        OWPML::CPType* p = (OWPML::CPType*)pSubObj;
                        OWPML::CRunType* r = NULL;

                        int paraCount = p->GetChildCount();
                        for (int n = 0; n < paraCount; n++) {
                            r = p->Getrun(n);

                            if (r) {
                                OWPML::CT* t = r->Gett(0);
                                if (t)
                                    return ProcessingTextElement(t);
                            }
                        }
                    }


                }

            }
        }
        break;
    }
    case ID_PARA_RectangleType:
    case ID_PARA_EllipseType:
    case ID_PARA_ArcType:
    case ID_PARA_PolygonType:
    case ID_PARA_CurveType:
        return ProcessingTextElement((OWPML::CT*)parent);
    default:
        break;
    }

    return u"";
}

/// 메타태그 텍스트 추출
std::u16string MetatagEX::ProcessingTextElement(OWPML::CT* pText)
{
    if (pText == NULL) {
        return u"";
    }

    std::u16string text, buff;
    UINT i = 0, j = 0, count = 0, len = 0;
    OWPML::Objectlist* pChildList = pText->GetObjectList();
    OWPML::CObject* pObject = NULL;

    if (pChildList) { // (#PCDATA | TITLEMARK | TAB | LINEBREAK | HYPEN | NBSPACE | FWSPACE)*
        count = (UINT)pChildList->size();
        for (i = 0; i < count; i++) {
            pObject = pChildList->at(i);
            if (pObject) {
                switch (pObject->GetID())
                {
                case ID_PARA_Char:
                {
                    OWPML::CChar* pChar = (OWPML::CChar*)pObject;
                    text.clear();
                    text.assign(Util::wstring_to_utf16(pChar->Getval()));
                    buff.clear();
                    len = text.size();
                    for (j = 0; j < len; j++) {
                        if (text.at(j) != HWPCH_LINE_BREAK && text.at(j) != HWPCH_PARA_BREAK && text.at(j) != HWPCH_TAB)
                            buff += text.at(j);
                    }
                    text.assign(buff);
                    break;
                }
                case ID_PARA_DrawText: {
                    text.clear();
                    if (pObject->HasChildList()) {
                        OWPML::Objectlist* pDrawTextChild = pObject->GetObjectList();
                        OWPML::Objectlist::iterator iter = pDrawTextChild->begin();

                        for (iter; iter != pDrawTextChild->end(); ++iter) {
                            OWPML::CObject* pObj = (*iter);
                            if (pObj && pObj->GetID() == ID_PARA_ParaListType) {
                                OWPML::Objectlist* pSubListChild = pObj->GetObjectList();
                                OWPML::Objectlist::iterator subIter = pSubListChild->begin();

                                for (subIter; subIter != pSubListChild->end(); ++subIter) {
                                    OWPML::CObject* pSubObj = (*subIter);
                                    if (pSubObj && pSubObj->GetID() == ID_PARA_PType) {
                                        OWPML::CPType* para = (OWPML::CPType*)pSubObj;
                                        OWPML::CRunType* run = NULL;

                                        int paraCount = para->GetChildCount();
                                        for (int n = 0; n < paraCount; n++) {
                                            run = para->Getrun(n);

                                            if (run) {
                                                OWPML::CT* t = run->Gett(0);
                                                if (t) {
                                                    text.assign(ProcessingTextElement(t));
                                                }
                                            }
                                        }
                                    }


                                }


                            }

                        }
                        return text;
                        break;
                    }

                }
                case ID_PARA_TitleMark:
                {
                    text.clear();
                    break;
                }
                case ID_PARA_Tab:
                {
                    text.clear();
                    text = ((char16_t)HWPCH_TAB);
                    break;
                }
                case ID_PARA_LineBreak:
                {
                    text.clear();
                    text = ((char16_t)HWPCH_LINE_BREAK);
                    break;
                }
                case ID_PARA_Hypen:
                {
                    text.clear();
                    text = ((char16_t)HWPCH_HYPHEN);
                    break;
                }
                case ID_PARA_FwSpace:
                {
                    text.clear();
                    text = ((char16_t)HWPCH_FIXED_WIDTH_SPACE);
                    break;
                }
                case ID_PARA_NbSpace:
                {
                    text.clear();
                    text = ((WCHAR)HWPCH_NON_BREAKING_SPACE);
                    break;
                }
                case ID_PARA_MarkpenBegin:
                {
                    text.clear();
                    break;
                }
                case ID_PARA_MarkpenEnd:
                {
                    text.clear();
                    break;
                }
                default:
                    text.clear();
                    break;
                }
            }

            if (!text.empty()) {
                return text;
            }
        } // for
    }
    return u"";
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