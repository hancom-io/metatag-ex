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
#include "./extend/XmlParser.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/ostreamwrapper.h"
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
            if(bAdd == true)
                index++;
            if(bReset == true)
                index = 0;
            return index;
        }

        static int GetTagIndex(bool bAdd = true, bool bReset = false)
        {
            static int index = 0;
            if(bAdd == true)
                index++;
            if(bReset == true)
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

        static std::vector<std::u16string>* GetMetatagContainer()
        {
            static std::vector<std::u16string> metatagContainer;

            return &metatagContainer;
        }

        void ExtractMetatag(std::string inputPath, std::string outputPath, Option option, Option dsc, bool bShowProgress, bool bHeaderOnly);
        void SortMetatag(std::string inputPath, std::string jsonPath, std::string outputPath, Option option, bool bShowProgress, bool bHeaderOnly);
        static void CalcPercentProc(int nFileCnt);

    private:
        void SearchHeader(std::string path, std::string filePath);
        void SearchSection(std::string path, std::string filePath);
        void SearchString(std::string filePath, DOMNode* node);
        void SortShape(DOMNode* node, std::string filePath);
        void SortTable(DOMNode* node, std::string filePath);
        void TraverseHeader(std::string path, rapidjson::Document& doc);
        void TraverseSection(std::string path, rapidjson::Document& doc);
        void TraverseTable(DOMNode* node, rapidjson::Document& doc);
        void TraverseShape(DOMNode* node, rapidjson::Document& doc);
        void ExtractShape(DOMNode* node, rapidjson::Document& doc);
        void ExtractString(DOMNode* node, rapidjson::Document& doc);
};