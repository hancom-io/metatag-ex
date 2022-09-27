/*
 * Copyright 2022 Hancom Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <fstream>
#include <vector>
#include <direct.h>
#include "CommandParser.h"
#include "EXdefine.h" // StringResource

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
            char cwd[MAX_PATH];
            _getcwd(cwd, sizeof(cwd));
            std::string helpFilePath = std::string(cwd) + StringResource::PathSeperator + "..\\MetatagEXHelp.txt";
            std::ifstream readFile(helpFilePath);
            if (readFile.is_open() == true) {
                std::string s;
                s.clear();
				int i = 0;
                do {
                    readFile.clear();
                    getline(readFile, s);

					if (i < 16) {
						i++;
						continue;
					}

                    WCHAR* converted = new WCHAR[s.length() + 1];
                    int nLen = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), s.length() + 1, NULL, NULL);
                    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), s.length() + 1, converted, nLen);
                    DWORD dwWritten;
                    HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
                    WriteConsoleW(output, converted, (DWORD)::wcslen(converted), &dwWritten, NULL);
                    delete converted;

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
