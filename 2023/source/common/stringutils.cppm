export module stringutils;

export import <cstring>;
export import <format>;

import types;

export namespace aoc
{
	template<typename... Args>
	[[nodiscard]] String StringFormat(const std::format_string<Args...> formatString, Args&&... args)
	{
		return std::format(formatString, std::forward<Args>(args)...);
	}

	Vector<String> Split(const String& string, const String& delimiterStr, Vector<u32>* outPositions = nullptr)
	{
		Vector<String> result;

		size_t searchPos = 0;
		while (searchPos < string.size())
		{
			size_t occurence = string.find(delimiterStr, searchPos);

			if (occurence == String::npos)
			{
				if (searchPos < string.size())
				{
					result.push_back(string.substr(searchPos));
					if (outPositions)
					{
						outPositions->push_back(static_cast<u32>(searchPos));
					}
				}
				break;
			}

			if (occurence != searchPos)
			{
				result.push_back(string.substr(searchPos, occurence - searchPos));
				if (outPositions)
				{
					outPositions->push_back(static_cast<u32>(searchPos));
				}
			}

			searchPos = occurence + delimiterStr.size();
		}

		return result;
	}

	Vector<String> Split(const String& string, const char delimiterChar)
	{
		return Split(string, String(1, delimiterChar));
	}
}