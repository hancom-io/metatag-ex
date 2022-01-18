#include <iostream>
#include "MetatagEX.h"
#include "CommandParser.h"
int main(int argc, char** argv) {
    MetatagEX metatagEX;
	//metatagEX.ExtractMetatag("C:\\Users\\hancom\\CMakeBuilds\\fd8840c2-8ffd-863e-a8c1-6ef7da577a33\\build\\x64-Debug(�⺻��)\\^.*.hwpx", "C:\\Users\\hancom\\CMakeBuilds\\fd8840c2-8ffd-863e-a8c1-6ef7da577a33\\build\\x64-Debug(�⺻��)\\result.json", Option::Console, Option::Ascend);
	//return 0;
	//metatagEX.SortMetatag("C:\\Users\\hancom\\CMakeBuilds\\fd8840c2-8ffd-863e-a8c1-6ef7da577a33\\build\\x64-Debug(�⺻��)\\^.*.hwpx", "C:\\Users\\hancom\\CMakeBuilds\\fd8840c2-8ffd-863e-a8c1-6ef7da577a33\\build\\x64-Debug(�⺻��)\\result.json", "C:\\Users\\hancom\\CMakeBuilds\\fd8840c2-8ffd-863e-a8c1-6ef7da577a33\\build\\x64-Debug(�⺻��)\\output.json", Option::File);
	//return 0;
	if (argc >= 1)
	{
		bool bFile = false;
		bool bDescend = false;
		bool bSourceList = false;
		bool bDestList = false;
		bool bShowProgress = false;
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
		if (bSourceList == true)
		{
			if (bFile == true)
			{
				metatagEX.ExtractMetatag(argv[argc - 2], argv[argc - 1], Option::File, bDescend ? Option::Descend : Option::Ascend, bShowProgress);
			}
			else
			{
				metatagEX.ExtractMetatag(argv[argc - 1], std::string(), Option::Console, bDescend ? Option::Descend : Option::Ascend, bShowProgress);
			}
		}
		else if (bDestList == true)
		{
			if (bFile == false)
			{
				metatagEX.SortMetatag(argv[argc - 2], argv[argc - 1], std::string(), Option::Console, bShowProgress);
			}
			else
			{
				metatagEX.SortMetatag(argv[argc - 3], argv[argc - 2], argv[argc - 1], Option::File, bShowProgress);
			}
		}
	}
	else
		std::cout << StringResource::InvalidInput << std::endl;
}
