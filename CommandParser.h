#include <vector>
#include <map>
#include <iostream>
#ifndef OS_UNIX
#include <atlstr.h>
#include <Windows.h>
#endif
#include <direct.h>
#include <string>
enum CommandDef
{
	ToFile,
	OrderDescend,
	SourceList,
	DestList,
	Help
};
namespace CommandWord
{
	static std::string ToJsonFile = "f";
	static std::string OrderDescend = "dsc";
	static std::string GetSourceList = "sl";
	static std::string GetDestList = "dl";
	static std::string Help = "help";
}
class CommandParser
{
public:
	CommandParser(int inputArgs, char** inputArr);
	~CommandParser();
	// 입력으로 들어온 문자열을 파싱한다.
	bool ParsingInput();
	// inputStr에 해당하는 문자가 있을경우 경로문자수를 결정한다.
	bool ConfigureOption(const std::string& inputStr, int pathCnt);
	bool ShowHelp();

	static std::map<CommandDef, bool>* GetCurCommandMap()
	{
		static std::map<CommandDef, bool> curCommandMap;

		return &curCommandMap;
	}

private:
	std::vector<const char*> m_inputStrVector;
	int m_nPathCnt;
};