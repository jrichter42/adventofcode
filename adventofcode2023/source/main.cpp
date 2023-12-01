import common;

import day1;

import <chrono>;

int main(int argc, char* argv[])
{
	using namespace aoc;
	using namespace std::chrono;

	Log(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");

	const auto start = high_resolution_clock::now();
	string result = ExecuteDay();
	const auto end = high_resolution_clock::now();

	Log("vvvvvvvvvvvvvvvvvvvvvvvvvvvvvv");
	Log("");
	Log(result);
	Log("");

	using DurationT = duration<float, std::milli>;
	const DurationT::rep duration = duration_cast<DurationT>(end - start).count();
	LogFormat("{} ms", duration);

	Log("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
	return 0;
}