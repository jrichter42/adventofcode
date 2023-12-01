import common;

import day1;

import <chrono>;

int main(int argc, char* argv[])
{
	using namespace aoc;
	using namespace std::chrono;

	Log(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");

	const auto startPart1 = high_resolution_clock::now();
	string resultPart1 = ExecutePart1();
	const auto endPart1 = high_resolution_clock::now();

	const auto startPart2 = high_resolution_clock::now();
	string resultPart2 = ExecutePart2();
	const auto endPart2 = high_resolution_clock::now();

	Log("vvvvvvvvvvvvvvvvvvvvvvvvvvvvvv");

	Log("Part1:");
	Log(resultPart1);

	Log("\nPart2:");
	Log(resultPart2);

	Log("");

	using DurationT = duration<float, std::milli>;
	const DurationT::rep durationPart1 = duration_cast<DurationT>(endPart1 - startPart1).count();
	const DurationT::rep durationPart2 = duration_cast<DurationT>(endPart2 - startPart2).count();

	LogFormat("Part1 {} ms, Part2 {} ms", durationPart1, durationPart2);

	Log("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
	return 0;
}