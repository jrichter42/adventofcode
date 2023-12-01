import common;

import day1;

import <chrono>;

int main(int argc, char* argv[])
{
	using namespace aoc;
	using namespace std::chrono;

	const auto start = high_resolution_clock::now();

	string result = ExecuteDay();

	const auto end = high_resolution_clock::now();

	using DurationT = duration<float, std::milli>;
	const DurationT::rep duration = duration_cast<DurationT>(end - start).count();

	LogFormat("Finished in {} ms. Result:", duration);
	Log(result);

	return 0;
}