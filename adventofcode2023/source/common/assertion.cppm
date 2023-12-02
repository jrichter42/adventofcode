module;

#include <cassert>

export module assertion;

import <utility>; // std::forward

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
}