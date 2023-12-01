export module day1;

import common;

export namespace aoc
{
	/*export string ExecuteDay()
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
	}*/

	export string ExecuteDay()
	{
		auto input = OpenInput("day1.txt");

		std::array<char, 10> numberChars;
		std::iota(numberChars.begin(), numberChars.end(), '0');

		std::array<string, 10> numberStrings{ "zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine" };

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

			size_t foundFirst = numberStrings.size();
			size_t foundLast = numberStrings.size();
			size_t foundFirstIdx = string::npos;
			size_t foundLastIdx = string::npos;
			for (size_t i = 0; i < numberStrings.size(); i++)
			{
				string numberStr = numberStrings[i];

				size_t idx = line.find(numberStr);

				if (idx != string::npos &&
					(foundFirstIdx == string::npos || idx < foundFirstIdx))
				{
					foundFirstIdx = idx;
					foundFirst = i;
				}

				idx = line.rfind(numberStr);

				if (idx != string::npos &&
					(foundLastIdx == string::npos || idx > foundLastIdx))
				{
					foundLastIdx = idx;
					foundLast = i;
				}
			}

			string valueStr;

			if (foundFirstIdx != string::npos && static_cast<int>(foundFirstIdx) < std::distance<string::const_iterator>(line.begin(), firstDigitIt))
			{
				valueStr.push_back((char)(int('0') + foundFirst));
			}
			else
			{
				valueStr.push_back(*firstDigitIt);
			}

			if (foundLastIdx != string::npos && static_cast<int>(foundLastIdx) > std::distance<string::const_reverse_iterator>(lastDigitIt, --(line.rend())))
			{
				valueStr.push_back(char(int('0') + foundLast));
			}
			else
			{
				valueStr.push_back(*lastDigitIt);
			}

			int value = std::stoi(valueStr);
			result += value;
		}

		return std::to_string(result);
	}
}