export module day4;

import common;

export namespace aoc
{
	struct Card
	{
		String Name;
		Vector<s32> WinningNumbers;
		Vector<s32> MyNumbers;

		u32 CardValue() const
		{
			u32 result = 0;
			for (s32 myNumber : MyNumbers)
			{
				auto it = std::find(WinningNumbers.begin(), WinningNumbers.end(), myNumber);
				if (it != WinningNumbers.end())
				{
					if (result == 0)
					{
						result = 1;
						continue;
					}
					result = result * 2;
				}
			}
			return result;
		}
	};

	export String ExecutePart1()
	{
		auto input = OpenInput("day4.txt");

		u32 result = 0;

		String line;
		while (std::getline(input, line))
		{
			const Vector<String> cardSplit = Split(line, ": ");
			const String& cardName = cardSplit[0];
			const String& cardContent = cardSplit[1];

			const Vector<String> cardContentSplit = Split(cardContent, " | ");
			const String& winningNumbersStr = cardContentSplit[0];
			const String& myNumbersStr = cardContentSplit[1];

			const Vector<String> winningNumberStrings = Split(winningNumbersStr, " ");
			const Vector<String> myNumberStrings = Split(myNumbersStr, " ");

			Card card = { .Name = cardName };

			for (const String& winningNumberStr : winningNumberStrings)
			{
				char* strtolEnd;
				const s32 winningNumberSigned = strtol(winningNumberStr.c_str(), &strtolEnd, 10);
				Assert(strtolEnd != winningNumberStr.c_str(), "no character was interpreted as number");

				card.WinningNumbers.push_back(winningNumberSigned);
			}

			for (const String& myNumberStr : myNumberStrings)
			{
				char* strtolEnd;
				const s32 myNumberSigned = strtol(myNumberStr.c_str(), &strtolEnd, 10);
				Assert(strtolEnd != myNumberStr.c_str(), "no character was interpreted as number");

				card.MyNumbers.push_back(myNumberSigned);
			}

			result += card.CardValue();
		}

		return std::to_string(result);
	}

	export String ExecutePart2()
	{
		return "";
	}
}