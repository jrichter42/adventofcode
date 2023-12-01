export module day1;

import common;

export namespace aoc
{
	export string ExecuteDay()
	{
		auto input = OpenInput("day1.txt");

		std::array<char, 10> numberChars;
		std::iota(numberChars.begin(), numberChars.end(), '0');

		int result = 0;
		while (input && input.eof() == false)
		{
			string line = Read<string>(input);

			if (line.empty())
			{
				continue;
			}

			auto firstDigitIt = std::find_first_of(line.begin(), line.end(), numberChars.begin(), numberChars.end());
			auto lastDigitIt = std::find_first_of(line.rbegin(), line.rend(), numberChars.begin(), numberChars.end());

			Assert(firstDigitIt != line.end());
			Assert(lastDigitIt != line.rend());

			string valueStr;
			valueStr.push_back(*firstDigitIt);
			valueStr.push_back(*lastDigitIt);

			int value = std::stoi(valueStr);
			result += value;
		}

		return std::to_string(result);
	}
}