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
        bool bSaveToOrigin = false;

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

        if (CommandParser::GetCurCommandMap()->find(CommandDef::HeaderOnly) != CommandParser::GetCurCommandMap()->end())
            bHeaderOnly = CommandParser::GetCurCommandMap()->find(CommandDef::HeaderOnly)->second;

        if (CommandParser::GetCurCommandMap()->find(CommandDef::ChangeTagName) != CommandParser::GetCurCommandMap()->end())
            bChangeTagName = CommandParser::GetCurCommandMap()->find(CommandDef::ChangeTagName)->second;

        if (CommandParser::GetCurCommandMap()->find(CommandDef::SaveToOrigin) != CommandParser::GetCurCommandMap()->end()) {
            bSaveToOrigin = CommandParser::GetCurCommandMap()->find(CommandDef::SaveToOrigin)->second;
            bChangeTagName = true;
        }

        if (bSourceList == true) {
            if (bFile == true) {
                metatagEX.ExtractMetatag(cmdParser.GetSrcFilePath(), cmdParser.GetSrcJsonPath(), Option::File, bDescend ? Option::Descend : Option::Ascend, bHeaderOnly);
            } else {
                metatagEX.ExtractMetatag(cmdParser.GetSrcFilePath(), std::string(), Option::Console, bDescend ? Option::Descend : Option::Ascend, bHeaderOnly);
            }
        } else if (bDestList == true) {
            if (bFile == false) {
                metatagEX.ClaasifyMetatag(cmdParser.GetSrcFilePath(), cmdParser.GetSrcJsonPath(), std::string(), Option::Console, bHeaderOnly);
            } else {
                metatagEX.ClaasifyMetatag(cmdParser.GetSrcFilePath(), cmdParser.GetSrcJsonPath(), cmdParser.GetDesJsonPath(), Option::File, bHeaderOnly);
            }
        } else if (bChangeTagName == true) {
            if (bFile == false) {
                metatagEX.ChangeMetatagName(cmdParser.GetSrcFilePath(), std::string(), cmdParser.GetOldTagName(), cmdParser.GetNewTagName(), bHeaderOnly);
            } else {
                metatagEX.ChangeMetatagName(cmdParser.GetSrcFilePath(), cmdParser.GetDesJsonPath(), cmdParser.GetOldTagName(), cmdParser.GetNewTagName(), bHeaderOnly, bSaveToOrigin);
            }
        }
    } else
        std::cout << StringResource::InvalidInput << std::endl;
}
