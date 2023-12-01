export module day1;

import common;

export namespace aoc
{
	export string ExecuteDay()
	{
		ifstream filestream = Open("input/day1.txt");
		return Read<string>(filestream);
	}
}