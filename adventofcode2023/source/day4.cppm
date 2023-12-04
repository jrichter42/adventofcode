export module day4;

import common;

export namespace aoc
{
	struct Card
	{
		String Name;
		u32 Number = 0;
		Vector<s32> WinningNumbers;
		Vector<s32> MyNumbers;
		mutable std::optional<u32> CachedWinningCount;

		Vector<s32> MyNumbersWinning() const
		{
			Vector<s32> result;
			for (s32 myNumber : MyNumbers)
			{
				auto it = std::find(WinningNumbers.begin(), WinningNumbers.end(), myNumber);
				if (it != WinningNumbers.end())
				{
					result.push_back(myNumber);
				}
			}
			return result;
		}

		u32 CardValue() const
		{
			u32 winningCount = NarrowSizeT(MyNumbersWinning().size());

			u32 result = 0;
			if (winningCount > 0)
			{
				result = 1;
				winningCount--;
			}

			while (winningCount-- > 0)
			{
				result *= 2;
			}

			return result;
		}

		u32 CardCount(const Vector<Card>& originalCards) const
		{
			if (CachedWinningCount.has_value())
			{
				return CachedWinningCount.value();
			}

			u32 winningCount = NarrowSizeT(MyNumbersWinning().size());

			u32 result = 1; // myself
			for (u32 i = 1; i <= winningCount; i++)
			{
				u32 cardToCopyNumber = Number + i;
				const Card& cardToCopy = originalCards[cardToCopyNumber - 1];
				Assert(cardToCopy.Number == cardToCopyNumber);

				result += cardToCopy.CardCount(originalCards);
			}

			CachedWinningCount = result;
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

			s32 cardNumber = std::stoi(Split(cardName, " ")[1]);

			Card card = { .Name = cardName, .Number = ToUnsigned(cardNumber) };

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
		auto input = OpenInput("day4.txt");

		Vector<Card> cards;

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

			s32 cardNumber = std::stoi(Split(cardName, " ")[1]);

			Card card = { .Name = cardName, .Number = ToUnsigned(cardNumber) };

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

			cards.push_back(std::move(card));
		}

		u32 result = 0;
		for (const Card& card : cards)
		{
			result += card.CardCount(cards);
		}

		return std::to_string(result);
	}
}