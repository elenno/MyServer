#include "helpFunctions.h"

HelpFunctions::HelpFunctions()
{

}

HelpFunctions::~HelpFunctions()
{

}

string HelpFunctions::tighten(const string& str)
{
	bool in_string = false;
	std::string tmp;
	for (std::string::const_iterator iter = str.begin(); iter != str.end(); iter++)
	{
		char ch = *iter;
		int a = (unsigned)(ch + 1);
		if (Between(a, 0, 256) && isspace(ch) && in_string == false)
		{
			continue;
		}

		tmp.insert(tmp.end(), ch);
		if (ch == '\"')
		{
			in_string = !in_string;
		}
	}
	return tmp;
}
