#include <fstream>
#include "CommandParser.h"
#include "EXdefine.h" // StringResource
CommandParser::CommandParser(int inputArgs, char** inputArr)
{
	for (int i = 1; i < inputArgs; ++i)
	{
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
	std::vector<const char*>::iterator iter = m_inputStrVector.begin();
	for (iter; iter != m_inputStrVector.end(); ++iter)
	{
		if (CommandWord::Help.compare(*iter) == 0)
		{
#ifdef OS_UNIX
			if (geteuid() == 0)
				flags |= ARCHIVE_EXTRACT_OWNER;

			char cwd[PATH_MAX];
			getcwd(cwd, sizeof(cwd));
#else
			char cwd[MAX_PATH];
			_getcwd(cwd, sizeof(cwd));
#endif
			std::string helpFilePath = std::string(cwd) + StringResource::PathSeperator + "MetatagEXHelp.txt";
			std::ifstream readFile(helpFilePath);
			if (readFile.is_open() == true)
			{
				std::string s;
				s.clear();
				do
				{
					readFile.clear();
					getline(readFile, s);
#ifndef OS_UNIX
					WCHAR* converted = new WCHAR[s.length() + 1];
					int nLen = MultiByteToWideChar(CP_UTF8, 0, s.c_str(),  s.length() + 1, NULL, NULL);
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
	// ConfigurePathCnt �� Ŀ�ǵ带 �м��Ͽ� ���° ���ڿ����� ������� �ľ��ؾ��Ѵ�.
	if (ConfigureOption(CommandWord::GetSourceList, 1) == false)
	{
		if (ConfigureOption(CommandWord::GetDestList, 2) == false)
		{
			std::cout << StringResource::InvalidInput.c_str() << std::endl;
			return false;
		}
		else
		{
			ConfigureOption(CommandWord::ToJsonFile, 3);
		}
	}
	else
	{
		ConfigureOption(CommandWord::ToJsonFile, 2);
	}

	int optionIndex = 0;
	int optionCnt = m_inputStrVector.size() - m_nPathCnt;
	std::vector<const char*>::iterator iter = m_inputStrVector.begin();
	for (iter; iter != m_inputStrVector.end(); ++iter)
	{
		if (optionIndex < optionCnt)
			continue;

	}

	return true;
}

// Option ���� Parsing �Ѵ�
bool CommandParser::ConfigureOption(const std::string& inputStr, int pathCnt)
{
	bool ret = false;
	std::vector<const char*>::iterator iter = m_inputStrVector.begin();
	for (iter; iter != m_inputStrVector.end(); ++iter)
	{
		// Path ���ڿ��� ����� �����Ѵ�.
		if (inputStr.compare(*iter) == 0)
		{
			m_nPathCnt = pathCnt;
			ret = true;
		}

		// Custom Define
		if (CommandWord::GetDestList.compare(*iter) == 0)
		{
			CommandParser::GetCurCommandMap()->insert(std::make_pair(CommandDef::DestList, true));
		}
		else if (CommandWord::GetSourceList.compare(*iter) == 0)
		{
			CommandParser::GetCurCommandMap()->insert(std::make_pair(CommandDef::SourceList, true));
		}
		else if (CommandWord::OrderDescend.compare(*iter) == 0)
		{
			CommandParser::GetCurCommandMap()->insert(std::make_pair(CommandDef::OrderDescend, true));
		}
		else if (CommandWord::ToJsonFile.compare(*iter) == 0)
		{
			CommandParser::GetCurCommandMap()->insert(std::make_pair(CommandDef::ToFile, true));
		}
		else if (CommandWord::Help.compare(*iter) == 0)
		{
			CommandParser::GetCurCommandMap()->insert(std::make_pair(CommandDef::Help, true));
		}
	}
	return ret;
}