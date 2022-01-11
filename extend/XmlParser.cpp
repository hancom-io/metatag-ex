#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMDocumentType.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMNodeIterator.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMText.hpp>
// #include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>

#include <iostream>
#include "../util/Util.h"

#include "./XmlParser.h"

// Xercess Parser
using namespace xercesc;
using namespace std;

int XmlParser::objectCount = 0;

XmlParser::XmlParser()
{
    try
    {
        if (objectCount == 0) {
            XMLPlatformUtils::Initialize();
        }
        objectCount++;
    }
    catch(XMLException& e)
    {
        char* message = XMLString::transcode(e.getMessage());
        cerr << "XML toolkit initialization error: " << message << endl;
        XMLString::release(&message);
    }

    m_XercesParser = new XercesDOMParser;

    xmlDoc = NULL;
    rootElement = NULL;

    xmlCreatedDoc = NULL;
}

XmlParser::~XmlParser()
{
    Release();

    try
    {
        objectCount--;
        
        if (objectCount <= 0) {
            XMLPlatformUtils::Terminate();
        }
    }
    catch(XMLException& e)
    {
        char* message = XMLString::transcode(e.getMessage());
        cerr << "XML ttolkit teardown error: " << message << endl;
        XMLString::release(&message);
    }
}

bool XmlParser::CrateDocument(const std::string& rootName)
{
    if (xmlCreatedDoc != NULL || rootElement != NULL) {
        return false;
    }
    if (rootName.empty() == true) {
        return false;
    }

    // DOMImplementation
    XMLCh* xmlStringPtr;

    xmlStringPtr = XMLString::transcode("");
    DOMImplementation* impl =  DOMImplementationRegistry::getDOMImplementation(xmlStringPtr);
    XMLString::release(&xmlStringPtr);
    xmlStringPtr = NULL;
    if (impl == NULL)
    {
        return false;
    }

    // DOMDocument & Root element
    try
    {
        xmlStringPtr = XMLString::transcode(rootName.c_str());
        xmlCreatedDoc = impl->createDocument(
            0,              // root element namespace URI.
            xmlStringPtr,   // root element name
            0);             // document type object (DTD).
        if (xmlCreatedDoc != NULL) {
            rootElement = xmlCreatedDoc->getDocumentElement();
        }

        XMLString::release(&xmlStringPtr);
        xmlStringPtr = NULL;
    }
    catch(XMLException& e) {
        Release();

        char* message = XMLString::transcode(e.getMessage());
        cerr << "XML ttolkit teardown error: " << message << endl;
        XMLString::release(&message);
    }
    return true;
}

DOMElement* XmlParser::CreateElement(const std::string& nodeName)
{
    if (nodeName.empty() == true || xmlCreatedDoc == NULL) {
        return NULL;
    }

    XMLCh* xmlNodeNamePtr = XMLString::transcode(nodeName.c_str());
    DOMElement*  element = NULL;
    try
    {
        element = xmlCreatedDoc->createElement(xmlNodeNamePtr);
    }
    catch (DOMException& e) {
        std::u16string u16nodeName = Util::string_to_utf16(nodeName);
    }
    XMLString::release(&xmlNodeNamePtr);
    return element;
}

void XmlParser::readFile(const std::string& configFile)
{
    m_XercesParser->setValidationScheme(XercesDOMParser::Val_Never);
    //m_XercesParser->setValidationScheme(XercesDOMParser::Val_Auto);
    m_XercesParser->setDoNamespaces(false);
    m_XercesParser->setDoSchema(false);
    m_XercesParser->setLoadExternalDTD(false);
    try
    {
        m_XercesParser->parse(configFile.c_str());
        xmlDoc = m_XercesParser->getDocument();
        if (xmlDoc != NULL) {
            rootElement = xmlDoc->getDocumentElement();
             if(rootElement) {

             }
             else {

             }
        }
        else {

        }
    }
    catch(xercesc::XMLException& e)
    {

    }
}


DOMElement* XmlParser::CreateElement(const std::u16string& nodeName)
{
    if (nodeName.empty() == true || xmlCreatedDoc == NULL) {
        return NULL;
    }

    DOMElement*  element = NULL;
    try
    {
        element = xmlCreatedDoc->createElement(reinterpret_cast<const XMLCh*>(nodeName.c_str()));
    }

    catch (DOMException& e) {

    }

    return element;
}

DOMNode* XmlParser::SelectSingleNode(string nodeName, DOMNode* node)
{
    if (nodeName.empty() == true || node == NULL)  {
        return NULL;
    }

    std::string name = ConvertToString(node->getNodeName());
    if(nodeName.compare(name) == 0)
    {
        return node;
    }
    DOMNodeList* list = node->getChildNodes();
    const XMLSize_t nodeCount = list->getLength();
    DOMNode* retNode = NULL;
    for(XMLSize_t i = 0; i < nodeCount; ++i)
    {
        DOMNode* childNode = list->item(i);
        name = ConvertToString(childNode->getNodeName());
        if(nodeName.compare(name) == 0)
        {
            retNode = childNode;
            break;
        }
    }
    return retNode;
}

void XmlParser::SelectNodes(string nodeName, DOMNode* node, std::list<DOMNode*>* nodelist)
{
    if (nodeName.empty() == true || node == NULL || nodelist == NULL)  {
        return;
    }

    string name = ConvertToString(node->getNodeName());
    if(nodeName.compare(name) == 0)
    {
        nodelist->push_back(node);
    }
    DOMNodeList* list = node->getChildNodes();
    const XMLSize_t nodeCount = list->getLength();
    for(XMLSize_t i = 0; i < nodeCount; ++i)
    {
        DOMNode* childNode = list->item(i);
        name = ConvertToString(childNode->getNodeName());
        if(nodeName.compare(name) == 0)
        {
            nodelist->push_back(childNode);
        }
        else if(nodeName.find(name) != std::string::npos)
        {
			if (nodeName.find('/') == std::string::npos)
				return;
            string temp(nodeName.begin() + nodeName.find_first_of('/') + 1, nodeName.end());
            SelectNodes(temp, childNode, nodelist);
        }
    }
}

bool XmlParser::ConvertToBool(const XMLCh* xmlString, bool defaultValue /* = false*/)
{
    bool value = defaultValue;
    char* transStr = XMLString::transcode(xmlString);
    if (transStr != NULL) {
#ifdef OS_UNIX
		value = (strcasecmp(transStr, "true") == 0) ? true : false;
#else
		value = (_stricmp(transStr, "true") == 0) ? true : false;
#endif

        XMLString::release(&transStr); 
    }

    return value;
}

std::string XmlParser::ConvertToString(const XMLCh* xmlString)
{
    std::string returnValue = "";

    char* transStr = XMLString::transcode(xmlString);
    if (transStr != NULL) {
        returnValue.assign(transStr);
        XMLString::release(&transStr);
    }

    return returnValue;
}

bool XmlParser::SetElementAttribute(DOMElement* element, const char* name, const char* value)
{
    if (element == NULL || name == NULL) {
        return false;
    }

    if (strlen(name) == 0) {
        return false;
    }

    XMLCh* attirbuteName;
    XMLCh* attirbuteValue;

    attirbuteName = XMLString::transcode(name);
    attirbuteValue = XMLString::transcode(value);
    element->setAttribute(attirbuteName, attirbuteValue);

    XMLString::release(&attirbuteName);
    XMLString::release(&attirbuteValue);

    return true;
}

xercesc::XercesDOMParser* XmlParser::GetXercesParser()
{
    return m_XercesParser;
}

DOMElement* XmlParser::GetRootElement()
{ 
    return rootElement;
}

void XmlParser::WriteXML(const std::string& filePath)
{
    //Return the first registered implementation that has the desired features. In this case, we are after a DOM implementation that has the LS feature... or Load/Save. 
    XMLCh* xmlStrFeatures = XMLString::transcode("LS");
    DOMImplementation *implementation = DOMImplementationRegistry::getDOMImplementation(xmlStrFeatures); 
    XMLString::release(&xmlStrFeatures);
    xmlStrFeatures = NULL;

    // Create a DOMLSSerializer which is used to serialize a DOM tree into an XML document. 
    DOMLSSerializer *serializer = ((DOMImplementationLS*)implementation)->createLSSerializer(); 

    // Make the output more human readable by inserting line feeds. 
    if (serializer->getDomConfig()->canSetParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true)) 
        serializer->getDomConfig()->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true); 

    // The end-of-line sequence of characters to be used in the XML being written out.  
    XMLCh* xmlStrLinefeed = XMLString::transcode("\r\n");
    serializer->setNewLine(xmlStrLinefeed);
    XMLString::release(&xmlStrLinefeed);
    xmlStrLinefeed = NULL;

    // Convert the path into Xerces compatible XMLCh*. 
    XMLCh *tempFilePath = XMLString::transcode(filePath.c_str()); 

    // Specify the target for the XML output. 
    XMLFormatTarget *formatTarget = new LocalFileFormatTarget(tempFilePath); 

    // Create a new empty output destination object. 
    DOMLSOutput *output = ((DOMImplementationLS*)implementation)->createLSOutput(); 

    // Set the stream to our target. 
    output->setByteStream(formatTarget); 

    // Write the serialized output to the destination. 
    serializer->write(xmlCreatedDoc, output); 

    // Cleanup. 
    serializer->release(); 
    XMLString::release(&tempFilePath); 
    delete formatTarget; 
    output->release(); 
}


void XmlParser::Release()
{
    if (rootElement != NULL) {
        rootElement = NULL;
    }
    if (xmlDoc != NULL && m_XercesParser != NULL) {
        m_XercesParser->endDocument();
        xmlDoc = NULL;
    }

    if (xmlCreatedDoc != NULL) {
        xmlCreatedDoc->release();
        xmlCreatedDoc = NULL;
    }

    if (m_XercesParser != NULL) {
        delete m_XercesParser;
        m_XercesParser = NULL;
    }
}