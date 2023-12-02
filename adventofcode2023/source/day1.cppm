export module day1;

import common;

export namespace aoc
{
	export String ExecutePart1()
	{
		auto input = OpenInput("day1.txt");

		std::array<char, 10> numberChars;
		std::iota(numberChars.begin(), numberChars.end(), '0');

		int result = 0;
		while (input && input.eof() == false)
		{
			String line = Read<String>(input);

			if (line.empty())
			{
				continue;
			}

			auto firstDigitIt = std::find_first_of(line.begin(), line.end(), numberChars.begin(), numberChars.end());
			auto lastDigitIt = std::find_first_of(line.rbegin(), line.rend(), numberChars.begin(), numberChars.end());

			Assert(firstDigitIt != line.end());
			Assert(lastDigitIt != line.rend());

			String valueStr;
			valueStr.push_back(*firstDigitIt);
			valueStr.push_back(*lastDigitIt);

			int value = std::stoi(valueStr);
			result += value;
		}

		return std::to_string(result);
	}

	export String ExecutePart2()
	{
		auto input = OpenInput("day1.txt");

		std::array<char, 10> numberChars;
		std::iota(numberChars.begin(), numberChars.end(), '0');

		std::array<String, 10> numberStrings{ "zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine" };

		int result = 0;
		while (input && input.eof() == false)
		{
			String line = Read<String>(input);

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
			size_t foundFirstIdx = String::npos;
			size_t foundLastIdx = String::npos;
			for (size_t i = 0; i < numberStrings.size(); i++)
			{
				String numberStr = numberStrings[i];

				size_t idx = line.find(numberStr);

				if (idx != String::npos &&
					(foundFirstIdx == String::npos || idx < foundFirstIdx))
				{
					foundFirstIdx = idx;
					foundFirst = i;
				}

				idx = line.rfind(numberStr);

				if (idx != String::npos &&
					(foundLastIdx == String::npos || idx > foundLastIdx))
				{
					foundLastIdx = idx;
					foundLast = i;
				}
			}

			String valueStr;

			if (foundFirstIdx != String::npos && static_cast<int>(foundFirstIdx) < std::distance<String::const_iterator>(line.begin(), firstDigitIt))
			{
				valueStr.push_back((char)(int('0') + foundFirst));
			}
			else
			{
				valueStr.push_back(*firstDigitIt);
			}

			if (foundLastIdx != String::npos && static_cast<int>(foundLastIdx) > std::distance<String::const_reverse_iterator>(lastDigitIt, --(line.rend())))
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