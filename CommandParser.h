#include <vector>
#include <map>
#include <iostream>
#ifndef OS_UNIX
	#include <atlstr.h>
	#include <Windows.h>
	#include <direct.h>
#endif
#include <string>
enum CommandDef
{
	ToFile,
	OrderDescend,
	SourceList,
	DestList,
	Help,
	ShowProgress,
};
namespace CommandWord
{
	static std::string ToJsonFile = "f";
	static std::string OrderDescend = "dsc";
	static std::string GetSourceList = "sl";
	static std::string GetDestList = "dl";
	static std::string Help = "help";
	static std::string ShowProgress = "pr";
}
class CommandParser
{
public:
	CommandParser(int inputArgs, char** inputArr);
	~CommandParser();
	// �Է����� ���� ���ڿ��� �Ľ��Ѵ�.
	bool ParsingInput();
	// inputStr�� �ش��ϴ� ���ڰ� ������� ��ι��ڼ��� �����Ѵ�.
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