module;

#pragma warning(push, 0)
#include <Windows.h>
#pragma warning(pop)

#include <cassert>

export module common;


export import <string>;
export using std::string;

export import <format>;
export using std::format;

export import <fstream>;
export using std::ifstream;

export import <algorithm>;
export import <numeric>;

export import <iterator>;
export import <vector>;
export import <array>;

import <iostream>;

export namespace aoc
{
	template<typename TCondition>
	auto constexpr Assert(TCondition&& condition)
	{
		if (std::forward<TCondition>(condition) == false)
		{
			__debugbreak();
		}
		//return assert(std::forward<TCondition>(condition));
	}

	template<typename TCondition>
	auto constexpr Assert(TCondition&& condition, auto message)
	{
		if (std::forward<TCondition>(condition) == false)
		{
			__debugbreak();
		}
		//return assert(std::forward<TCondition>(condition));
	}

	void Log(const string& message)
	{
		std::cout << message << std::endl;
		OutputDebugStringA(message.c_str());
		OutputDebugStringA("\n");
	}

	template<typename... Args>
	void LogFormat(std::format_string<Args...> messageFormat, Args&&... args)
	{
		using FormatStringT = std::format_string<Args...>;
		string formattedMessage = format(std::forward<FormatStringT>(messageFormat), std::forward<Args>(args)...);
		Log(formattedMessage);
	}

	[[nodiscard]] ifstream Open(const string& filepath)
	{
        ifstream result(filepath);
        if (!result.is_open())
        {
            Log("Error: Couldn't open file");
            Log(filepath);
        }

        return result;

	}

	[[nodiscard]] ifstream OpenInput(const string& filepathRelativeToInput)
	{
		return Open(string("input/") + filepathRelativeToInput);
	}

	template<typename DataT>
	bool Read(ifstream& filestream, DataT& data)
	{
		filestream >> data;

		if (filestream.fail())
		{
			Log("Error: Failed to read from filestream");

			switch (filestream.rdstate())
			{
				case ifstream::badbit:
				{
					Log("badbit: irrecoverable stream error");
				} break;
				case ifstream::failbit:
				{
					Log("failbit: input/output operation failed (formatting or extraction error)");
				} break;
				case ifstream::eofbit:
				{
					Log("eofbit: associated input sequence has reached end-of-file");
				} break;
			}

			return false;
		}

		return true;
	}

	template<typename DataT>
	[[nodiscard]] DataT Read(ifstream& filestream)
	{
		DataT result;
		Read(filestream, result);
		return result;
	}
}