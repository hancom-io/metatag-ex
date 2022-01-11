#include "MetatagEX.h"
#include <regex>
#ifdef OS_UNIX
#include <dirent.h>
#endif // OS_UNIX
#include <fstream>

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
    while(GetFileIndex(false, false) != nFileCnt)
    {
        if(curIndex != GetFileIndex(false, false))
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

void MetatagEX::SortMetatag(std::string inputPath, std::string jsonPath, std::string outputPath, Option option)
{
	Initialize();

    rapidjson::Document jsonDoc;

	if (Util::IsExistFile(jsonPath) == false)	
	{
		std::cout << StringResource::NoJsonFile << std::endl;
		return;
	}
    std::ifstream ifs(jsonPath);
    rapidjson::IStreamWrapper isw(ifs);
    jsonDoc.ParseStream(isw);
    rapidjson::StringBuffer buffer {};
    rapidjson::Writer<rapidjson::StringBuffer> writer { buffer };
    jsonDoc.Accept(writer);

    if(jsonDoc.HasParseError())
    {
        std::cout << "Invalid Input" << std::endl;

        return;
    }

    rapidjson::Value::MemberIterator itr = jsonDoc.MemberBegin();
    for(itr; itr != jsonDoc.MemberEnd(); ++itr)
    {
        MetatagEX::GetFindMetatagVector()->push_back(Util::string_to_utf16(itr->value.GetString()));
    }

    vector<std::string> files;
    std::string regexStr = Util::getdir(inputPath, files);
    vector<std::string>::iterator iter_file = files.begin();
    std::thread CountingThread(CalcPercentProc, files.size());
    for(iter_file; iter_file != files.end(); ++iter_file)
    {
        std::regex reg;
        try
        {
            reg = std::regex(regexStr);
        }
        catch(std::regex_error::exception e)
        {
            reg = std::regex("^.*.hwpx$");
        }

        GetFileIndex(true, false);

        std::smatch match;
        if(std::regex_search(*iter_file, match, reg) == false)
        {
            continue;
        }
		std::string unzipPath = std::string(inputPath + StringResource::PathSeperator + "UnzipTemp");
        if(Util::isDirectory(unzipPath.c_str()))
        {
            Util::removeDirectory(unzipPath.c_str());
#ifdef OS_UNIX
            Util::createDirectory(Util::string_to_utf16(unzipPath.c_str()));
#else
			CreateDirectory(unzipPath.c_str(), NULL);
#endif
        }
        else
        {
#ifdef OS_UNIX
            Util::createDirectory(Util::string_to_utf16(unzipPath.c_str()));
#else
			CreateDirectory(unzipPath.c_str(), NULL);
#endif
        }
        
#ifndef OS_UNIX
		char path[MAX_PATH];
#else
        char path[PATH_MAX];
#endif
        sprintf(path, "%s", std::string(inputPath + StringResource::PathSeperator + *iter_file).c_str());
		if (Util::extract(path, unzipPath.c_str()) == -1)
		{
			std::cout << std::string(path) + StringResource::PathSeperator + *iter_file + StringResource::ExtractionFailed << std::endl;
			return;
		}

		SearchHeader(unzipPath, inputPath + StringResource::PathSeperator + *iter_file);
		SearchSection(unzipPath, inputPath + StringResource::PathSeperator + *iter_file);
        Util::removeDirectory(unzipPath.c_str());
    }
    CountingThread.join();
    if(option == Option::Console)
    {
        std::vector<std::pair<std::string, std::u16string>>::iterator iter_sorted = MetatagEX::GetSortedMetatag()->begin();
        for(iter_sorted; iter_sorted != MetatagEX::GetSortedMetatag()->end(); ++iter_sorted)
        {
            std::cout << iter_sorted->first << " : " << Util::utf16_to_string(iter_sorted->second) << std::endl;
        }
    }
    else if(option == Option::File)
    {
        std::vector<std::pair<std::string, std::u16string>>::iterator iter_sorted = MetatagEX::GetSortedMetatag()->begin();
        jsonDoc.SetObject();
        for(iter_sorted; iter_sorted != MetatagEX::GetSortedMetatag()->end(); ++iter_sorted)
        {
            auto& allocator = jsonDoc.GetAllocator();
            rapidjson::Value key(iter_sorted->first.c_str(), allocator);
            rapidjson::Value val(Util::utf16_to_string(iter_sorted->second).c_str(), allocator);
            jsonDoc.AddMember(key, val, allocator);
        }
        std::ofstream ofs(outputPath);
        rapidjson::OStreamWrapper osw(ofs);

        rapidjson::Writer<rapidjson::OStreamWrapper> writer(osw);
        jsonDoc.Accept(writer);
        ofs.close();
    }
}

void MetatagEX::SearchHeader(std::string path, std::string filePath)
{
    XmlParser headerParser;
    headerParser.readFile(std::string(path + StringResource::PathSeperator + "Contents" + StringResource::PathSeperator + "header.xml").c_str());
    DOMNode* pNodeHeader = XmlParser::SelectSingleNode(Defines::NODE_HEAD, headerParser.GetRootElement());
    std::list<DOMNode*> docMetatagList;
    headerParser.SelectNodes(Defines::NODE_HMETATAG, pNodeHeader, &docMetatagList);
    std::list<DOMNode*>::iterator iter = docMetatagList.begin();
    for(iter; iter != docMetatagList.end(); ++iter)
    {
        SearchString(filePath, *iter);
    }
}

void MetatagEX::SearchString(std::string filePath, DOMNode* node)
{
    std::u16string contents;
    contents.assign(reinterpret_cast<const char16_t*>(node->getTextContent()));
    int pos = contents.find(u"#");
    int endpos = contents.find(u",", pos);
    if(endpos == std::string::npos)
        endpos = contents.find(u"\"", pos);
    while(pos != std::string::npos)
    {
        std::u16string token = contents.substr(pos, endpos - pos);
        
        std::vector<std::u16string>::iterator iter_find = std::find(MetatagEX::GetFindMetatagVector()->begin(), MetatagEX::GetFindMetatagVector()->end(), token);
        if(iter_find != MetatagEX::GetFindMetatagVector()->end())
        {
#ifdef OS_UNIX
            char fullPath[PATH_MAX];
            realpath(filePath.c_str(), fullPath);
#else // OS_UNIX
			char fullPath[MAX_PATH];
			char* fileName;
			GetFullPathName(filePath.c_str(), MAX_PATH, fullPath, &fileName);
#endif
			MetatagEX::GetSortedMetatag()->push_back(make_pair(fullPath, *iter_find));
        }

        pos = contents.find(u"#", endpos);
        endpos = contents.find(u",", pos);
        if(endpos == std::string::npos)
            endpos = contents.find(u"\"", pos);
    }
}

void MetatagEX::SearchSection(std::string path, std::string filePath)
{
    bool isEndOfSection = false;
    int sectionIndex = 0;
    while (isEndOfSection == false) {
        std::string sectionFilePath = path + StringResource::PathSeperator + "Contents";
        char filename[50] = {0, };
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
            for(iter_tbl; iter_tbl != tblList.end(); ++iter_tbl)
            {
                SortTable(*iter_tbl, filePath);
            }
            tblList.clear();
            SortShape(*itr, filePath);
        }
    }
}

void MetatagEX::SortTable(DOMNode* node, std::string filePath)
{
    std::list<DOMNode*> tblMetatagList;
    std::list<DOMNode*>::iterator iter_tag;
    XmlParser::SelectNodes(Defines::NODE_PMETATAG, node, &tblMetatagList);
    for(iter_tag = tblMetatagList.begin(); iter_tag != tblMetatagList.end(); ++iter_tag)
    {
        SearchString(filePath, *iter_tag);
    }
    std::list<DOMNode*> cellList;
    std::list<DOMNode*>::iterator iter_cell;
    XmlParser::SelectNodes(Defines::NODE_SUBLIST, node, &cellList);
    for(iter_cell = cellList.begin(); iter_cell != cellList.end(); ++iter_cell)
    {
        XMLCh* attritbuteID = XMLString::transcode("metatag");
        DOMNode* attribute = (*iter_cell)->getAttributes()->getNamedItem(attritbuteID);
        if(attribute != NULL)
        {
            std::string metaTagsStr = XmlParser::ConvertToString(attribute->getTextContent());
            int pos = metaTagsStr.find("#");
            int endpos = metaTagsStr.find(",", pos);
            if(endpos == std::string::npos)
                endpos = metaTagsStr.find("\"", pos);
            while(pos != std::string::npos)
            {
                std::string token = metaTagsStr.substr(pos, endpos - pos);
                
                std::vector<std::u16string>::iterator iter_find = std::find(MetatagEX::GetFindMetatagVector()->begin(), MetatagEX::GetFindMetatagVector()->end(), Util::string_to_utf16(token));
                if(iter_find != MetatagEX::GetFindMetatagVector()->end())
                {
#ifdef OS_UNIX
                    char fullPath[PATH_MAX];
                    realpath(filePath.c_str(), fullPath);
#else // OS_UNIX
					char fullPath[MAX_PATH];
					char* fileName;
					GetFullPathName(filePath.c_str(), MAX_PATH, fullPath, &fileName);
#endif
					MetatagEX::GetSortedMetatag()->push_back(make_pair(fullPath, *iter_find));
                }

                pos = metaTagsStr.find("#", endpos);
                endpos = metaTagsStr.find(",", pos);
                if(endpos == std::string::npos)
                    endpos = metaTagsStr.find("\"", pos);
            }
        }
        XMLString::release(&attritbuteID);
        std::list<DOMNode*> nodelist;
        std::list<DOMNode*>::iterator itr;
        XmlParser::SelectNodes(Defines::NODE_PARA, *iter_cell, &nodelist);
        for(itr = nodelist.begin(); itr != nodelist.end(); ++itr)
        {
            std::list<DOMNode*> tblList;
            XmlParser::SelectNodes(Defines::NODE_TABLE, *itr, &tblList);
            std::list<DOMNode*>::iterator iter_tbl = tblList.begin();
            for(iter_tbl; iter_tbl != tblList.end(); ++iter_tbl)
            {
                SortTable(*iter_tbl, filePath);
            }
            tblList.clear();
            SortShape(*itr, filePath);
        }
    }
}

void MetatagEX::SortShape(DOMNode* node, std::string filePath)
{
    std::list<DOMNode*> shapeList;
    XmlParser::SelectNodes(Defines::NODE_RECT, node, &shapeList);
    std::list<DOMNode*>::iterator iter_shape = shapeList.begin();
    for(iter_shape; iter_shape != shapeList.end(); ++iter_shape)
    {
        SearchString(filePath, *iter_shape);
    }
    shapeList.clear();
    XmlParser::SelectNodes(Defines::NODE_PICTURE, node, &shapeList);
    for(iter_shape = shapeList.begin(); iter_shape != shapeList.end(); ++iter_shape)
    {
        SearchString(filePath, *iter_shape);
    }
    shapeList.clear();
    XmlParser::SelectNodes(Defines::NODE_ELLIPSE, node, &shapeList);
    for(iter_shape = shapeList.begin(); iter_shape != shapeList.end(); ++iter_shape)
    {
        SearchString(filePath, *iter_shape);
    }
    shapeList.clear();
    XmlParser::SelectNodes(Defines::NODE_LINE, node, &shapeList);
    for(iter_shape = shapeList.begin(); iter_shape != shapeList.end(); ++iter_shape)
    {
        SearchString(filePath, *iter_shape);
    }
    shapeList.clear();
    XmlParser::SelectNodes(Defines::NODE_ARC, node, &shapeList);
    for(iter_shape = shapeList.begin(); iter_shape != shapeList.end(); ++iter_shape)
    {
        SearchString(filePath, *iter_shape);
    }
    shapeList.clear();
    XmlParser::SelectNodes(Defines::NODE_POLYGON, node, &shapeList);
    for(iter_shape = shapeList.begin(); iter_shape != shapeList.end(); ++iter_shape)
    {
        SearchString(filePath, *iter_shape);
    }
    shapeList.clear();
    XmlParser::SelectNodes(Defines::NODE_CURV, node, &shapeList);
    for(iter_shape = shapeList.begin(); iter_shape != shapeList.end(); ++iter_shape)
    {
        SearchString(filePath, *iter_shape);
    }
    shapeList.clear();
    XmlParser::SelectNodes(Defines::NODE_CONNECTLINE, node, &shapeList);
    for(iter_shape = shapeList.begin(); iter_shape != shapeList.end(); ++iter_shape)
    {
        SearchString(filePath, *iter_shape);
    }
    shapeList.clear();
}

void MetatagEX::ExtractMetatag(std::string inputPath, std::string outputPath, Option option, Option dsc)
{
	Initialize();

    vector<std::string> files;
    std::string regexStr = Util::getdir(inputPath, files);
    vector<std::string>::iterator iter_file = files.begin();
    rapidjson::Document jsonDoc;
    jsonDoc.SetObject();
    std::thread CountingThread(CalcPercentProc, files.size());
    for(iter_file; iter_file != files.end(); ++iter_file)
    {
        std::regex reg;
        try
        {
            reg = std::regex(regexStr);
        }
        catch(std::regex_error::exception e)
        {
            reg = std::regex("^.*.hwpx$");
        }

        MetatagEX::GetFileIndex(true, false);

        std::smatch match;
        if(std::regex_search(*iter_file, match, reg) == false)
        {
            continue;
        }
		std::string unzipPath = std::string(inputPath + StringResource::PathSeperator + "UnzipTemp");

        if(Util::isDirectory(unzipPath.c_str()))
        {
            Util::removeDirectory(unzipPath.c_str());
#ifdef OS_UNIX
            Util::createDirectory(Util::string_to_utf16(unzipPath.c_str()));
#else
			::CreateDirectory(unzipPath.c_str(), NULL);
#endif
        }
        else
        {
#ifdef OS_UNIX
            Util::createDirectory(Util::string_to_utf16(unzipPath.c_str()));
#else
			::CreateDirectory(unzipPath.c_str(), NULL);
#endif
        }

#ifndef OS_UNIX
		char path[MAX_PATH];
#else
        char path[PATH_MAX];
#endif // OS_UNIX
        sprintf(path, "%s", std::string(inputPath + StringResource::PathSeperator + *iter_file).c_str());
		if (Util::extract(path, unzipPath.c_str()) == -1)
		{
			std::cout << std::string(path) + StringResource::PathSeperator + *iter_file + StringResource::ExtractionFailed << std::endl;
			return;
		}

        TraverseHeader(unzipPath, jsonDoc);
        TraverseSection(unzipPath, jsonDoc);

        Util::removeDirectory(unzipPath.c_str());
    }

    CountingThread.join();
    // Order
    if(dsc == Option::Descend)
    {
        std::sort(MetatagEX::GetMetatagContainer()->begin(), MetatagEX::GetMetatagContainer()->end(), std::greater<std::u16string>());
        std::unique(MetatagEX::GetMetatagContainer()->begin(), MetatagEX::GetMetatagContainer()->end());
    }
    else
    {
        std::sort(MetatagEX::GetMetatagContainer()->begin(), MetatagEX::GetMetatagContainer()->end(), std::less<std::u16string>());
        std::unique(MetatagEX::GetMetatagContainer()->begin(), MetatagEX::GetMetatagContainer()->end());
    }

    std::vector<std::u16string>::iterator iter;
    if(option == Option::Console)
    {
        for(iter = MetatagEX::GetMetatagContainer()->begin(); iter != MetatagEX::GetMetatagContainer()->end(); ++iter)
        {
            char valName[50] = {0, };
            snprintf(valName, sizeof(char) * 50, "MetaTag%d", GetTagIndex());
            std::cout << valName << " : " << Util::utf16_to_string(*iter) << endl;
        }
    }
    else if(option == Option::File)
    {
        for(iter = MetatagEX::GetMetatagContainer()->begin(); iter != MetatagEX::GetMetatagContainer()->end(); ++iter)
        {
            char keyName[50] = {0, };
            snprintf(keyName, sizeof(char) * 50, "MetaTag%d", GetTagIndex());
            auto& allocator = jsonDoc.GetAllocator();
            rapidjson::Value key;
            key.SetString(keyName, allocator);
            rapidjson::Value val(Util::utf16_to_string(*iter).c_str(), allocator);
            jsonDoc.AddMember(key, val, allocator);
        }
        std::ofstream ofs(outputPath);
        rapidjson::OStreamWrapper osw(ofs);

        rapidjson::Writer<rapidjson::OStreamWrapper> writer(osw);
        jsonDoc.Accept(writer);
        ofs.close();
    }
}
void MetatagEX::TraverseHeader(std::string path, rapidjson::Document& doc)
{
    XmlParser headerParser;
    headerParser.readFile(std::string(path + StringResource::PathSeperator + "Contents" + StringResource::PathSeperator + "header.xml").c_str());
    DOMNode* pNodeHeader = XmlParser::SelectSingleNode(Defines::NODE_HEAD, headerParser.GetRootElement());
    std::list<DOMNode*> docMetatagList;
    headerParser.SelectNodes(Defines::NODE_HMETATAG, pNodeHeader, &docMetatagList);
    std::list<DOMNode*>::iterator iter = docMetatagList.begin();
    for(iter; iter != docMetatagList.end(); ++iter)
    {
        ExtractString(*iter, doc);
    }
}

void MetatagEX::TraverseSection(std::string path, rapidjson::Document& doc)
{
    bool isEndOfSection = false;
    int sectionIndex = 0;
    while (isEndOfSection == false) {
        std::string sectionFilePath = path + StringResource::PathSeperator + "Contents";
        char filename[50] = {0, };
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
            for(iter_tbl; iter_tbl != tblList.end(); ++iter_tbl)
            {
                TraverseTable(*iter_tbl, doc);
            }
            tblList.clear();
            TraverseShape(*itr, doc);
        }
    }
}

void MetatagEX::TraverseShape(DOMNode* node, rapidjson::Document& doc)
{
    std::list<DOMNode*> shapeList;
    XmlParser::SelectNodes(Defines::NODE_RECT, node, &shapeList);
    std::list<DOMNode*>::iterator iter_shape = shapeList.begin();
    for(iter_shape; iter_shape != shapeList.end(); ++iter_shape)
    {
        ExtractShape(*iter_shape, doc);
    }
    shapeList.clear();
    XmlParser::SelectNodes(Defines::NODE_PICTURE, node, &shapeList);
    for(iter_shape = shapeList.begin(); iter_shape != shapeList.end(); ++iter_shape)
    {
        ExtractShape(*iter_shape, doc);
    }
    shapeList.clear();
    XmlParser::SelectNodes(Defines::NODE_ELLIPSE, node, &shapeList);
    for(iter_shape = shapeList.begin(); iter_shape != shapeList.end(); ++iter_shape)
    {
        ExtractShape(*iter_shape, doc);
    }
    shapeList.clear();
    XmlParser::SelectNodes(Defines::NODE_LINE, node, &shapeList);
    for(iter_shape = shapeList.begin(); iter_shape != shapeList.end(); ++iter_shape)
    {
        ExtractShape(*iter_shape, doc);
    }
    shapeList.clear();
    XmlParser::SelectNodes(Defines::NODE_ARC, node, &shapeList);
    for(iter_shape = shapeList.begin(); iter_shape != shapeList.end(); ++iter_shape)
    {
        ExtractShape(*iter_shape, doc);
    }
    shapeList.clear();
    XmlParser::SelectNodes(Defines::NODE_POLYGON, node, &shapeList);
    for(iter_shape = shapeList.begin(); iter_shape != shapeList.end(); ++iter_shape)
    {
        ExtractShape(*iter_shape, doc);
    }
    shapeList.clear();
    XmlParser::SelectNodes(Defines::NODE_CURV, node, &shapeList);
    for(iter_shape = shapeList.begin(); iter_shape != shapeList.end(); ++iter_shape)
    {
        ExtractShape(*iter_shape, doc);
    }
    shapeList.clear();
    XmlParser::SelectNodes(Defines::NODE_CONNECTLINE, node, &shapeList);
    for(iter_shape = shapeList.begin(); iter_shape != shapeList.end(); ++iter_shape)
    {
        ExtractShape(*iter_shape, doc);
    }
    shapeList.clear();
}

void MetatagEX::TraverseTable(DOMNode* node, rapidjson::Document& doc)
{
    std::list<DOMNode*> tblMetatagList;
    std::list<DOMNode*>::iterator iter_tag;
    XmlParser::SelectNodes(Defines::NODE_PMETATAG, node, &tblMetatagList);
    for(iter_tag = tblMetatagList.begin(); iter_tag != tblMetatagList.end(); ++iter_tag)
    {
        ExtractString(*iter_tag, doc);
    }
    std::list<DOMNode*> cellList;
    std::list<DOMNode*>::iterator iter_cell;
    XmlParser::SelectNodes(Defines::NODE_SUBLIST, node, &cellList);
    for(iter_cell = cellList.begin(); iter_cell != cellList.end(); ++iter_cell)
    {
        XMLCh* attritbuteID = XMLString::transcode("metatag");
        DOMNode* attribute = (*iter_cell)->getAttributes()->getNamedItem(attritbuteID);
        if(attribute != NULL)
        {
            std::string metaTagsStr = XmlParser::ConvertToString(attribute->getTextContent());
            int pos = metaTagsStr.find("#");
            int endpos = metaTagsStr.find(",", pos);
            if(endpos == std::string::npos)
                endpos = metaTagsStr.find("\"", pos);
            while(pos != std::string::npos)
            {
                std::string token = metaTagsStr.substr(pos, endpos - pos);
                MetatagEX::GetMetatagContainer()->push_back(Util::string_to_utf16(token));
                pos = metaTagsStr.find("#", endpos);
                endpos = metaTagsStr.find(",", pos);
                if(endpos == std::string::npos)
                    endpos = metaTagsStr.find("\"", pos);
            }
        }
        XMLString::release(&attritbuteID);
        std::list<DOMNode*> nodelist;
        std::list<DOMNode*>::iterator itr;
        XmlParser::SelectNodes(Defines::NODE_PARA, *iter_cell, &nodelist);
        for(itr = nodelist.begin(); itr != nodelist.end(); ++itr)
        {
            std::list<DOMNode*> tblList;
            XmlParser::SelectNodes(Defines::NODE_TABLE, *itr, &tblList);
            std::list<DOMNode*>::iterator iter_tbl = tblList.begin();
            for(iter_tbl; iter_tbl != tblList.end(); ++iter_tbl)
            {
                TraverseTable(*iter_tbl, doc);
            }
            tblList.clear();
            TraverseShape(*itr, doc);
        }
    }
}

void MetatagEX::ExtractShape(DOMNode* node, rapidjson::Document& doc)
{
    std::list<DOMNode*> shapeMetatagList;
    std::list<DOMNode*>::iterator iter_tag;
    XmlParser::SelectNodes(Defines::NODE_PMETATAG, node, &shapeMetatagList);
    for(iter_tag = shapeMetatagList.begin(); iter_tag != shapeMetatagList.end(); ++iter_tag)
    {
        ExtractString(*iter_tag, doc);
    }
}

void MetatagEX::ExtractString(DOMNode* node, rapidjson::Document& doc)
{
    std::u16string contents;
    contents.assign(reinterpret_cast<const char16_t*>(node->getTextContent()));
    int pos = contents.find(u"#");
    int endpos = contents.find(u",", pos);
    if(endpos == std::string::npos)
        endpos = contents.find(u"\"", pos);
    while(pos != std::string::npos)
    {
        std::u16string token = contents.substr(pos, endpos - pos);
        MetatagEX::GetMetatagContainer()->push_back(token);
        pos = contents.find(u"#", endpos);
        endpos = contents.find(u",", pos);
        if(endpos == std::string::npos)
            endpos = contents.find(u"\"", pos);
    }
}