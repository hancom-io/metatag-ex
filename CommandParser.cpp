#include <fstream>
#include "CommandParser.h"
#include "EXdefine.h" // StringResource
#ifdef OS_UNIX
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <archive.h>
#include <archive_entry.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#endif // OS_UNIX
CommandParser::CommandParser(int inputArgs, char** inputArr)
{
    for (int i = 1; i < inputArgs; ++i) {
        m_inputStrVector.push_back(inputArr[i]);
    }
    m_nPathCnt = 0;
}

CommandParser::~CommandParser()
{
    m_inputStrVector.clear();
    CommandParser::GetCurCommandMap()->clear();
}

bool CommandParser::ShowHelp()
{
    for(auto iter : m_inputStrVector){
        if (CommandWord::Help.compare(iter) == 0) {
#ifdef OS_UNIX
            char cwd[PATH_MAX];
            getcwd(cwd, sizeof(cwd));
            std::string helpFilePath = std::string(cwd) + StringResource::PathSeperator + ".." + StringResource::PathSeperator + "MetatagEXHelp.txt";
#else
            char cwd[MAX_PATH];
            _getcwd(cwd, sizeof(cwd));
            std::string helpFilePath = std::string(cwd) + StringResource::PathSeperator + "MetatagEXHelp.txt";
#endif
            std::ifstream readFile(helpFilePath);
            if (readFile.is_open() == true) {
                std::string s;
                s.clear();
                do {
                    readFile.clear();
                    getline(readFile, s);
#ifndef OS_UNIX
                    WCHAR* converted = new WCHAR[s.length() + 1];
                    int nLen = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), s.length() + 1, NULL, NULL);
                    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), s.length() + 1, converted, nLen);
                    DWORD dwWritten;
                    HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
                    WriteConsoleW(output, converted, (DWORD)::wcslen(converted), &dwWritten, NULL);
                    delete converted;
#else	// OS_UNIX
                    std::cout << s << std::endl;
#endif
                    std::cout << std::endl;
                } while (readFile);
                readFile.close();
            }
            return true;
        }
    }
    return false;
}

bool CommandParser::ParsingInput()
{
    bool hasSL = false;
    bool hasDL = false;
    bool hasSrcJson = false;
    bool hasOldTagName = false;
    bool isChageTagName = false;

    for(auto& iter : m_inputStrVector){

        if ((iter.find(".hwpx") != std::string::npos)) {
            m_SrcFilePath = iter;
        }

        if ((iter.find(".json") != std::string::npos) && (hasSrcJson == false && isChageTagName == false)) {
            m_SrcJsonPath = iter;
            hasSrcJson = true;
        } else if ((iter.find(".json") != std::string::npos) && (hasSrcJson == true || isChageTagName == true)){
            m_DesJsonPath = iter;
        }

        if ((CommandParser::GetCurCommandMap()->find(CommandDef::ChangeTagName) != CommandParser::GetCurCommandMap()->end() &&
            (iter.find("#") != std::string::npos) &&
            hasOldTagName == false)) {
            m_OldTagName = iter;
            hasOldTagName = true;
        } else if ((CommandParser::GetCurCommandMap()->find(CommandDef::ChangeTagName) != CommandParser::GetCurCommandMap()->end() &&
            (iter.find("#") != std::string::npos) &&
            hasOldTagName == true)) {
            m_newTagName = iter;
        }

        // Custom Define
        if (CommandWord::GetDestList.compare(iter) == 0) {
            hasDL = true;
            CommandParser::GetCurCommandMap()->insert(std::make_pair(CommandDef::DestList, true));
        } else if (CommandWord::GetSourceList.compare(iter) == 0) {
            hasSL = true;
            CommandParser::GetCurCommandMap()->insert(std::make_pair(CommandDef::SourceList, true));
        } else if (CommandWord::OrderDescend.compare(iter) == 0) {
            CommandParser::GetCurCommandMap()->insert(std::make_pair(CommandDef::OrderDescend, true));
        } else if (CommandWord::ToJsonFile.compare(iter) == 0) {
            CommandParser::GetCurCommandMap()->insert(std::make_pair(CommandDef::ToFile, true));
        } else if (CommandWord::Help.compare(iter) == 0) {
            CommandParser::GetCurCommandMap()->insert(std::make_pair(CommandDef::Help, true));
        } else if (CommandWord::ShowProgress.compare(iter) == 0) {
            CommandParser::GetCurCommandMap()->insert(std::make_pair(CommandDef::ShowProgress, true));
        } else if (CommandWord::HeaderOnly.compare(iter) == 0) {
            CommandParser::GetCurCommandMap()->insert(std::make_pair(CommandDef::HeaderOnly, true));
        } else if (CommandWord::ChangeTagName.compare(iter) == 0) {
            CommandParser::GetCurCommandMap()->insert(std::make_pair(CommandDef::ChangeTagName, true));
            isChageTagName = true;
        } 

        if (hasDL && hasSL)
            return false;
    }
    return true;
}
