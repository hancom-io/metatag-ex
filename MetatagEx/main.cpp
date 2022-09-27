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

#include <iostream>
#include "MetatagEX.h"
#include "CommandParser.h"


int main(int argc, char** argv) {
    MetatagEX metatagEX;
    if (argc >= 1) {
        bool bFile = false;
        bool bDescend = false;
        bool bSourceList = false;
        bool bDestList = false;
        bool bShowProgress = false;
        bool bHeaderOnly = false;
        bool bChangeTagName = false;

        CommandParser cmdParser(argc, argv);
        if (cmdParser.ShowHelp() == true)
            return 0;
        if (cmdParser.ParsingInput() == false) {
            std::cout << StringResource::InvalidInput.c_str() << std::endl;
            return 0;
        }
        if (CommandParser::GetCurCommandMap()->find(CommandDef::ToFile) != CommandParser::GetCurCommandMap()->end())
            bFile = CommandParser::GetCurCommandMap()->find(CommandDef::ToFile)->second;
        if (CommandParser::GetCurCommandMap()->find(CommandDef::OrderDescend) != CommandParser::GetCurCommandMap()->end())
            bDescend = CommandParser::GetCurCommandMap()->find(CommandDef::OrderDescend)->second;
        if (CommandParser::GetCurCommandMap()->find(CommandDef::SourceList) != CommandParser::GetCurCommandMap()->end())
            bSourceList = CommandParser::GetCurCommandMap()->find(CommandDef::SourceList)->second;
        if (CommandParser::GetCurCommandMap()->find(CommandDef::DestList) != CommandParser::GetCurCommandMap()->end())
            bDestList = CommandParser::GetCurCommandMap()->find(CommandDef::DestList)->second;
        if (CommandParser::GetCurCommandMap()->find(CommandDef::ShowProgress) != CommandParser::GetCurCommandMap()->end())
            bShowProgress = CommandParser::GetCurCommandMap()->find(CommandDef::ShowProgress)->second;
        if (CommandParser::GetCurCommandMap()->find(CommandDef::HeaderOnly) != CommandParser::GetCurCommandMap()->end())
            bHeaderOnly = CommandParser::GetCurCommandMap()->find(CommandDef::HeaderOnly)->second;
        if (CommandParser::GetCurCommandMap()->find(CommandDef::ChangeTagName) != CommandParser::GetCurCommandMap()->end())
            bChangeTagName = CommandParser::GetCurCommandMap()->find(CommandDef::ChangeTagName)->second;

        if (bSourceList == true) {
            if (bFile == true) {
                metatagEX.ExtractMetatag(cmdParser.GetSrcFilePath(), cmdParser.GetSrcJsonPath(), Option::File, bDescend ? Option::Descend : Option::Ascend, bShowProgress, bHeaderOnly);
            } else {
                metatagEX.ExtractMetatag(cmdParser.GetSrcFilePath(), std::string(), Option::Console, bDescend ? Option::Descend : Option::Ascend, bShowProgress, bHeaderOnly);
            }
        } else if (bDestList == true) {
            if (bFile == false) {
                metatagEX.SortMetatag(cmdParser.GetSrcFilePath(), cmdParser.GetSrcJsonPath(), std::string(), Option::Console, bShowProgress, bHeaderOnly);
            } else {
                metatagEX.SortMetatag(cmdParser.GetSrcFilePath(), cmdParser.GetSrcJsonPath(), cmdParser.GetDesJsonPath(), Option::File, bShowProgress, bHeaderOnly);
            }
        } else if (bChangeTagName == true) {
            if (bFile == false) {
                metatagEX.ChangeMetatagName(cmdParser.GetSrcFilePath(), std::string(), cmdParser.GetOldTagName(), cmdParser.GetNewTagName(), bHeaderOnly);
            } else {
                metatagEX.ChangeMetatagName(cmdParser.GetSrcFilePath(), cmdParser.GetDesJsonPath(), cmdParser.GetOldTagName(), cmdParser.GetNewTagName(), bHeaderOnly);
            }
        }
    } else
        std::cout << StringResource::InvalidInput << std::endl;
}
