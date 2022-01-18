#include <iostream>
namespace StringResource
{
	const std::string InvalidInput = "Invalid Input";
	const std::string NoJsonFile = "Input Json File Not Exists";
	const std::string ExtractionFailed = " Extraction Failed!";
	const std::string Complete = "Complete!";
#ifdef OS_UNIX
	const std::string PathSeperator = "/";
#else // OS_UNIX
	const std::string PathSeperator = "\\";
#endif
};

namespace Defines
{
	const std::string NODE_HEAD = "hh:head";
	const std::string NODE_HMETATAG = "hh:metaTag";
	const std::string NODE_PMETATAG = "hp:metaTag";
	const std::string NODE_SECTION = "hs:sec";
	const std::string NODE_PARA = "hp:p";
	const std::string NODE_TABLE = "hp:run/hp:tbl";
	const std::string NODE_SUBLIST = "hp:tr/hp:tc/hp:subList";
	const std::string NODE_RECT = "hp:run/hp:rect";
	const std::string NODE_PICTURE = "hp:run/hp:pic";
	const std::string NODE_ELLIPSE = "hp:run/hp:ellipse";
	const std::string NODE_LINE = "hp:run/hp:line";
	const std::string NODE_ARC = "hp:run/hp:arc";
	const std::string NODE_POLYGON = "hp:run/hp:polygon";
	const std::string NODE_CURV = "hp:run/hp:curv";
	const std::string NODE_CONNECTLINE = "hp:run/hp:connectLine";
};

enum Option
{
	File,
	Console,
	Descend,
	Ascend,
	Source,
	Destination,
};