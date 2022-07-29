#ifndef _METATAGEX_
#define _METATAGEX_

#include <thread>
#include <map>
#include <stack>
#include "./util/Util.h"
#include "EXdefine.h"

#include "./opensource/rapidjson/include/rapidjson/document.h"
#include "./opensource/rapidjson/include/rapidjson/writer.h"
#include "./opensource/rapidjson/include/rapidjson/stringbuffer.h"
#include "./opensource/rapidjson/include/rapidjson/prettywriter.h"
#include "./opensource/rapidjson/include/rapidjson/istreamwrapper.h"
#include "./opensource/rapidjson/include/rapidjson/ostreamwrapper.h"
#include "./opensource/hwpx-owpml-model/OWPMLUtil/StringDef.h"
#include "./opensource/hwpx-owpml-model/OWPMLApi/OWPMLDocument.h"
#include "./opensource/hwpx-owpml-model/OWPML/Base/Object.h"
#include "./opensource/hwpx-owpml-model/OWPML/Class/classinclude.h"



class MetatagEX
{
public:
    MetatagEX()
    {

    }
    ~MetatagEX()
    {

    }
    typedef struct Metatag {
        std::string tagName = "";
        std::string filePath = "";
        std::string fileName = "";
        std::string objectType = "";
        std::string contentText = "";
        OWPML::CObject* object = NULL;
    }METATAG;

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

    std::vector<std::pair<std::string, std::vector<METATAG>>> tagContainer;
    std::vector<std::string> tagNameContainer;
    std::vector<std::pair<std::string, std::string>> sortedMetatag;

    void ExtractMetatag(std::string inputPath, std::string outputPath, Option option, Option dsc, bool bShowProgress, bool bHeaderOnly);
    void SortMetatag(std::string inputPath, std::string jsonPath, std::string outputPath, Option option, bool bShowProgress, bool bHeaderOnly);
    bool ChangeMetatagName(std::string inputPath, std::string jsonPath, std::string oldTagName, std::string newTagName, bool bHeaderOnly);
    static void CalcPercentProc(int nFileCnt);

private:
    void TraverseHeader(std::string path, std::string srcfilePath, std::vector<METATAG>* metatag, OWPML::COwpmlDocumnet* document = NULL);
    void TraverseSection(std::string path, std::string srcfilePath, std::vector<METATAG>* metatag, OWPML::COwpmlDocumnet* document = NULL);
    std::u16string GetObjectTypeText(unsigned int id);
    bool ImportMetatagFromJson(std::string path, rapidjson::Document &jsonDoc);

    void SearchHeader(std::string path, std::string srcfilePath, OWPML::COwpmlDocumnet* document = NULL);
    void SearchSection(std::string path, std::string srcfilePath, OWPML::COwpmlDocumnet* document = NULL);
    void SearchString(std::string srcfilePath, std::string tag);
    void ExtractString(std::string srcfilePath, OWPML::CObject* object, std::vector<METATAG>* metatag);

    void GetMetaTagObject(OWPML::CObject* searchObject, OWPML::Objectlist& result);
    void GetMetaTagObject(OWPML::CObject* searchObject, std::multimap<std::u16string, OWPML::CObject*>& result);
    bool SetMetaTagName(OWPML::CObject* object, std::u16string srcTagName, std::u16string destTagName);
    std::u16string GetMetaTagName(OWPML::CObject* object);
    std::u16string GetMetaTagContent(OWPML::CObject* object);
    OWPML::CPType* ConvertCelltoPara(OWPML::CTc* tc);
    std::u16string ProcessingTextElement(OWPML::CObject* object, int type = OWPML::FIELDTYPE::FT_NONE);

};

#endif //_METATAGEX_