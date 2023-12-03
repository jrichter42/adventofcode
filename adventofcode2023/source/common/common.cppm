export module common;

export import types;

export import logging;
export import assertion;
export import stringutils;
export import fileutils;

export import <algorithm>;

export namespace aoc
{
	constexpr bool IsDebug()
	{
#ifdef _DEBUG
		return true;
#elif _RELEASE
		return false;
#endif // DEBUG || RELEASE
	}
}