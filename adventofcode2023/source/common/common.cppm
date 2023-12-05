export module common;

export import types;

export import logging;
export import assertion;
export import stringutils;
export import fileutils;

export import <algorithm>;
export import <thread>;
export import <future>;

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