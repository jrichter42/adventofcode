module;

#pragma warning(push, 0)
#include <Windows.h>
#pragma warning(pop)

export module logging;

import types;
import stringutils;

import <iostream>; // std::cout

export namespace aoc
{
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
		String formattedMessage = StringFormat(std::forward<FormatStringT>(messageFormat), std::forward<Args>(args)...);
		Log(formattedMessage);
	}
}