#include <iostream>
#include "MetatagEX.h"
#include "CommandParser.h"
int main(int argc, char** argv) {
    MetatagEX metatagEX;
	if (argc >= 1)
	{
		bool bFile = false;
		bool bDescend = false;
		bool bSourceList = false;
		bool bDestList = false;
		bool bShowProgress = false;
		bool bHeaderOnly = false;
		CommandParser cmdParser(argc, argv);
		if (cmdParser.ShowHelp() == true)
			return 0;
		if (cmdParser.ParsingInput() == false)
		{
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
		if (bSourceList == true)
		{
			if (bFile == true)
			{
				metatagEX.ExtractMetatag(argv[argc - 2], argv[argc - 1], Option::File, bDescend ? Option::Descend : Option::Ascend, bShowProgress, bHeaderOnly);
			}
			else
			{
				metatagEX.ExtractMetatag(argv[argc - 1], std::string(), Option::Console, bDescend ? Option::Descend : Option::Ascend, bShowProgress, bHeaderOnly);
			}
		}
		else if (bDestList == true)
		{
			if (bFile == false)
			{
				metatagEX.SortMetatag(argv[argc - 2], argv[argc - 1], std::string(), Option::Console, bShowProgress, bHeaderOnly);
			}
			else
			{
				metatagEX.SortMetatag(argv[argc - 3], argv[argc - 2], argv[argc - 1], Option::File, bShowProgress, bHeaderOnly);
			}
		}
	}
	else
		std::cout << StringResource::InvalidInput << std::endl;
}
