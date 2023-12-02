export module stringutils;

export import <cstring>;
export import <string>;

import types;

export namespace aoc
{
	vector<String> Split(const String& string, const String& delimiterStr)
	{
		vector<String> result;

		size_t searchPos = 0;
		while (searchPos < string.size())
		{
			size_t occurence = string.find(delimiterStr, searchPos);
			if (occurence == String::npos)
			{
				if (searchPos < (string.size() - delimiterStr.size()))
				{
					result.push_back(string.substr(searchPos));
				}
				break;
			}
			result.push_back(string.substr(searchPos, occurence - searchPos));
			searchPos = occurence + delimiterStr.size();
		}

		return result;
	}

	vector<String> Split(const String& string, const char delimiterChar)
	{
		return Split(string, String(1, delimiterChar));
	}
}