#ifndef _METATAGEX_
#define _METATAGEX_

#include <iostream>
#include <thread>
#include <map>
#include <set>
#include <list>
#include <vector>
#include <algorithm>
#include <limits.h>
#include <stdlib.h>
#include "./util/Util.h"
#ifdef US_UNIX
#include "./extend/XmlParser.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/ostreamwrapper.h"
#else
#include "./opensource/rapidjson/include/rapidjson/document.h"
#include "./opensource/rapidjson/include/rapidjson/writer.h"
#include "./opensource/rapidjson/include/rapidjson/stringbuffer.h"
#include "./opensource/rapidjson/include/rapidjson/prettywriter.h"
#include "./opensource/rapidjson/include/rapidjson/istreamwrapper.h"
#include "./opensource/rapidjson/include/rapidjson/ostreamwrapper.h"

#include "./opensource/hwpx-owpml-model/OWPMLApi/OWPMLDocument.h"
#include "./opensource/hwpx-owpml-model/OWPMLUtil/HncStringUtil.h"

// owpml stdafx에 정의된 헤더들.. 정리 필요
#include <cstdlib>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <deque>
#include <stdio.h>
#include <queue>
#include <stack>
#include <algorithm>
#include <complex>
#include <memory>
#include "./opensource/hwpx-owpml-model/OWPML/Base/Object.h"
#include "./opensource/hwpx-owpml-model/OWPML/Class/classinclude.h"


#endif
#include "EXdefine.h"

class MetatagEX
{
    public:
        MetatagEX()
        {

        }
        ~MetatagEX()
        {

        }

        void Initialize();

        static int GetFileIndex(bool bAdd = true, bool bReset = false)
        {
            static int index = 0;
            if (bAdd == true)
                index++;
            if (bReset == true)
                index = 0;
            return index;
        }

        static int GetTagIndex(bool bAdd = true, bool bReset = false)
        {
            static int index = 0;
            if (bAdd == true)
                index++;
            if (bReset == true)
                index = 0;
            return index;
        }

        static std::vector<std::pair<std::string, std::u16string>>* GetSortedMetatag()
        {
            static std::vector<std::pair<std::string, std::u16string>> sortedMetatag;

            return &sortedMetatag;
        }

        static std::vector<std::u16string>* GetFindMetatagVector()
        {
            static std::vector<std::u16string> findMetatagVector;

            return &findMetatagVector;
        }

        static std::vector<std::pair<std::u16string, std::map<std::string, std::string>>>* GetMetatagContainer()
        {
            static std::vector<std::pair<std::u16string, std::map<std::string, std::string>>> metatagContainer;

            return &metatagContainer;
        }

        void ExtractMetatag(std::string inputPath, std::string outputPath, Option option, Option dsc, bool bShowProgress, bool bHeaderOnly);
        void SortMetatag(std::string inputPath, std::string jsonPath, std::string outputPath, Option option, bool bShowProgress, bool bHeaderOnly);
        static void CalcPercentProc(int nFileCnt);
    private:

        void TraverseHeader(std::string path, std::string srcfilePath, OWPML::COwpmlDocumnet* document = NULL);
        void TraverseSection(std::string path, std::string srcfilePath, OWPML::COwpmlDocumnet* document = NULL);
        std::u16string GetObjectTypeText(unsigned int id);
        bool ImportMetatagFromJson(std::string path, rapidjson::Document &jsonDoc);

#ifdef OS_UNIX
        void SearchString(std::string srcfilePath, DOMNode* node);
        void SortShape(DOMNode* node, std::string srcfilePath);
        void SortTable(DOMNode* node, std::string srcfilePath);

        void TraverseTable(DOMNode* node, std::string srcfilePath);
        void TraverseShape(DOMNode* node, std::string srcfilePath);
        void ExtractShape(DOMNode* node, std::string srcfilePath);
        void ExtractString(std::string srcfilePath, DOMNode* node, std::string origin);
#else
        void SearchHeader(std::string path, std::string srcfilePath, OWPML::COwpmlDocumnet* document = NULL);
        void SearchSection(std::string path, std::string srcfilePath, OWPML::COwpmlDocumnet* document = NULL);
        void SearchString(std::string srcfilePath, std::u16string tag);
        void ExtractString(std::string srcfilePath, std::u16string tag, std::string origin, std::string contentText);

        void GetMetaTagObject(OWPML::CObject* searchObject, OWPML::Objectlist& result);
        void GetMetaTagObject(OWPML::CObject* searchObject, std::multimap<std::u16string, OWPML::CObject*>& result);
        //void SetMetaTagName(OWPML::CObject* object, std::u16string srcTagName, std::u16string destTagName);
        std::u16string GetMetaTagName(OWPML::CObject* object);
        std::u16string GetMetaTagContent(OWPML::CObject* object);
        OWPML::CPType* ConvertCelltoPara(OWPML::CTc* tc);
        std::u16string ProcessingTextElement(OWPML::CT* pText);
#endif
};

#endif //_METATAGEX_