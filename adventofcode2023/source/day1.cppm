export module day1;

import common;

export namespace aoc
{
	export string ExecuteDay()
	{
		auto input = OpenInput("day1.txt");
		return Read<string>(input);
	}
}