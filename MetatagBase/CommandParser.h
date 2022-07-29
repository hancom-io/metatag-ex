#ifndef _METATAGEX_COMMANDPARSER_
#define _METATAGEX_COMMANDPARSER_

#include <Windows.h>
#include <map>
#include <string>

enum CommandDef
{
    ToFile,
    OrderDescend,
    SourceList,
    DestList,
    Help,
    ShowProgress,
    HeaderOnly,
    ChangeTagName
};
namespace CommandWord
{
    static std::string ToJsonFile = "-f";
    static std::string OrderDescend = "-dsc";
    static std::string GetSourceList = "-sl";
    static std::string GetDestList = "-dl";
    static std::string Help = "-o";
    static std::string ShowProgress = "-pr";
    static std::string HeaderOnly = "-doc";
    static std::string ChangeTagName = "-c";
}
class CommandParser
{
public:
    CommandParser(int inputArgs, char** inputArr);
    ~CommandParser();

    bool ParsingInput();
    bool ShowHelp();

    static std::map<CommandDef, bool>* GetCurCommandMap()
    {
        static std::map<CommandDef, bool> curCommandMap;

        return &curCommandMap;
    }

    std::string GetSrcFilePath() { return m_SrcFilePath; }
    std::string GetSrcJsonPath() { return m_SrcJsonPath; }
    std::string GetDesJsonPath() { return m_DesJsonPath; }
    std::string GetOldTagName() { return m_OldTagName; }
    std::string GetNewTagName() { return m_newTagName; }

private:
    std::vector<std::string> m_inputStrVector;
    int m_nPathCnt;

    std::string m_SrcFilePath;
    std::string m_SrcJsonPath; 
    std::string m_DesJsonPath;
    std::string m_OldTagName;
    std::string m_newTagName;

    
};
#endif //_METATAGEX_COMMANDPARSER_