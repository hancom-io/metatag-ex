#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <iostream>
#include <list>
using namespace std;
using namespace xercesc;

class XmlParser
{
    public:
        XmlParser();
        ~XmlParser();

    // =====================================================
    // static
    public:
        static DOMNode* SelectSingleNode(string nodeName, DOMNode* node);
        static void SelectNodes(string nodeName, DOMNode* node, std::list<DOMNode*>* nodelist);
        static bool ConvertToBool(const XMLCh* xmlString, bool defaultValue = false);
        static std::string ConvertToString(const XMLCh* xmlString);

        static bool SetElementAttribute(DOMElement* element, const char* name, const char* value);
    private:
        static int objectCount;

    // =====================================================

    public:
        bool CrateDocument(const std::string& rootName);

        DOMElement* CreateElement(const std::string& nodeName);
        DOMElement* CreateElement(const std::u16string& nodeName);

        void readFile(const std::string& configFile);
        XercesDOMParser* GetXercesParser();
        DOMElement* GetRootElement();

        void WriteXML(const std::string& filePath);

    private:
        xercesc::XercesDOMParser* m_XercesParser;

        xercesc::DOMDocument* xmlDoc;
        xercesc::DOMElement* rootElement;

        xercesc::DOMDocument* xmlCreatedDoc;

        void Release();
};