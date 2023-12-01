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

	Log("------------------------------");

	const auto startPart2 = high_resolution_clock::now();
	string resultPart2 = ExecutePart2();
	const auto endPart2 = high_resolution_clock::now();

	Log("vvvvvvvvvvvvvvvvvvvvvvvvvvvvvv");

	LogFormat("\nPart1:\n{}\n\nPart2:\n{}\n\n", resultPart1, resultPart2);

	using DurationT = duration<float, std::milli>;
	const DurationT::rep durationPart1 = duration_cast<DurationT>(endPart1 - startPart1).count();
	const DurationT::rep durationPart2 = duration_cast<DurationT>(endPart2 - startPart2).count();

	LogFormat("Part1: {} ms\nPart2: {} ms", durationPart1, durationPart2);

	Log("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
	return 0;
}