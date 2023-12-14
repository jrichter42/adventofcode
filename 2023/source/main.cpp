import common;

import day14;

constexpr bool EXECUTE_PART1 = true;
constexpr bool EXECUTE_PART2 = true;

int main(int argc, char* argv[])
{
	using namespace aoc;
	using namespace std::chrono;

	Log(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");

	String resultPart1;
	const auto startPart1 = high_resolution_clock::now();
	if constexpr (EXECUTE_PART1)
	{
		resultPart1 = ExecutePart1();
	}
	const auto endPart1 = high_resolution_clock::now();

	if constexpr (EXECUTE_PART1 && EXECUTE_PART2)
	{
		Log("------------------------------");
	}

	String resultPart2;
	const auto startPart2 = high_resolution_clock::now();
	if constexpr (EXECUTE_PART2)
	{
		resultPart2 = ExecutePart2();
	}
	const auto endPart2 = high_resolution_clock::now();

	Log("vvvvvvvvvvvvvvvvvvvvvvvvvvvvvv");

	if constexpr (EXECUTE_PART1)
	{
		LogFormat("\nPart1:\n{}\n", resultPart1);
	}
	if constexpr (EXECUTE_PART2)
	{
		LogFormat("\nPart2:\n{}\n", resultPart2);
	}
	Log("");

	using DurationT = duration<float, std::milli>;
	if constexpr (EXECUTE_PART1)
	{
		const DurationT::rep durationPart1 = duration_cast<DurationT>(endPart1 - startPart1).count();
		LogFormat("Part1: {} ms", durationPart1);
	}
	if constexpr (EXECUTE_PART2)
	{
		const DurationT::rep durationPart2 = duration_cast<DurationT>(endPart2 - startPart2).count();
		LogFormat("Part2: {} ms", durationPart2);
	}

	Log("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
	return 0;
}