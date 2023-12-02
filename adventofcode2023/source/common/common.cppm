module;

#pragma warning(push, 0)
#include <Windows.h>
#pragma warning(pop)

#include <cassert>

export module common;

export import types;
export import stringutils;

export import <format>;
export import <algorithm>;

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

	void Log(const String& message)
	{
		std::cout << message << std::endl;
		OutputDebugStringA(message.c_str());
		OutputDebugStringA("\n");
	}

	template<typename... Args>
	void LogFormat(std::format_string<Args...> messageFormat, Args&&... args)
	{
		using FormatStringT = std::format_string<Args...>;
		String formattedMessage = format(std::forward<FormatStringT>(messageFormat), std::forward<Args>(args)...);
		Log(formattedMessage);
	}

	[[nodiscard]] ifstream Open(const String& filepath)
	{
        ifstream result(filepath);
        if (!result.is_open())
        {
            Log("Error: Couldn't open file");
            Log(filepath);
        }

        return result;

	}

	[[nodiscard]] ifstream OpenInput(const String& filepathRelativeToInput)
	{
		return Open(String("input/") + filepathRelativeToInput);
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