#ifndef _METATAGEX_
#define _METATAGEX_

#ifdef UNICODE
#undef UNICODE
#endif

#include <thread>
#include <map>
#include <stack>
#include "util/Util.h"
#include "EXdefine.h"

#include "opensource/rapidjson/include/rapidjson/document.h"
#include "opensource/rapidjson/include/rapidjson/writer.h"
#include "opensource/rapidjson/include/rapidjson/stringbuffer.h"
#include "opensource/rapidjson/include/rapidjson/prettywriter.h"
#include "opensource/rapidjson/include/rapidjson/istreamwrapper.h"
#include "opensource/rapidjson/include/rapidjson/ostreamwrapper.h"
#include "opensource/hwpx-owpml-model/OWPMLUtil/HncBaseStringDef.h"
#include "opensource/hwpx-owpml-model/OWPMLUtil/StringDef.h"
#include "opensource/hwpx-owpml-model/OWPMLApi/OWPMLDocument.h"
#include "opensource/hwpx-owpml-model/OWPML/Base/Object.h"
#include "opensource/hwpx-owpml-model/OWPML/Class/classinclude.h"



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

    std::vector<std::pair<std::string, std::vector<METATAG>>> tagContainer;
    std::vector<std::string> tagNameContainer;
    std::vector<std::pair<std::string, std::string>> sortedMetatag;

    void ExtractMetatag(std::string inputPath, std::string outputPath, Option option, Option dsc, bool bShowProgress, bool bHeaderOnly);
    void SortMetatag(std::string inputPath, std::string jsonPath, std::string outputPath, Option option, bool bShowProgress, bool bHeaderOnly);
    bool ChangeMetatagName(std::string inputPath, std::string jsonPath, std::string oldTagName, std::string newTagName, bool bHeaderOnly);

private:
    void TraverseHeader(std::string srcfilePath, std::vector<METATAG>* metatag, OWPML::COwpmlDocumnet* document);
    void TraverseSection(std::string srcfilePath, std::vector<METATAG>* metatag, OWPML::COwpmlDocumnet* document);
    void ExtractString(std::string srcfilePath, OWPML::CObject* object, std::vector<METATAG>* metatag);

    void SearchHeader(std::string srcfilePath, OWPML::COwpmlDocumnet* document);
    void SearchSection(std::string srcfilePath, OWPML::COwpmlDocumnet* document);
    void SearchString(std::string srcfilePath, std::string tag);

    std::u16string GetObjectTypeText(unsigned int id);
    bool ImportMetatagFromJson(std::string path, rapidjson::Document &jsonDoc);

    void GetMetaTagObject(OWPML::CObject* searchObject, OWPML::Objectlist& result);
    void GetMetaTagObject(OWPML::CObject* searchObject, std::multimap<std::u16string, OWPML::CObject*>& result);

    bool SetMetaTagName(OWPML::CObject* object, std::u16string srcTagName, std::u16string destTagName);
    std::u16string GetMetaTagName(OWPML::CObject* object);

    std::u16string GetMetaTagContent(OWPML::CObject* object);
    OWPML::CPType* ConvertCelltoPara(OWPML::CTc* tc);
    std::u16string ProcessingTextElement(OWPML::CObject* object, int type = OWPML::FIELDTYPE::FT_NONE);

};

#endif //_METATAGEX_