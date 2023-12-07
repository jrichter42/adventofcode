export module day7;

import common;

export namespace aoc
{
	enum class Card
	{
		Invalid,
		two = 2,
		three,
		four,
		five,
		six,
		seven,
		eight,
		nine,
		Ten,
		Jack,
		Queen,
		King,
		Ace
	};

	Card CharToCard(char c)
	{
		switch (c)
		{
			case '2': return Card::two;
			case '3': return Card::three;
			case '4': return Card::four;
			case '5': return Card::five;
			case '6': return Card::six;
			case '7': return Card::seven;
			case '8': return Card::eight;
			case '9': return Card::nine;
			case 'T': return Card::Ten;
			case 'J': return Card::Jack;
			case 'Q': return Card::Queen;
			case 'K': return Card::King;
			case 'A': return Card::Ace;
		}
		return Card::Invalid;
	}

	enum class HandType
	{
		Invalid,
		HighCard,
		OnePair,
		TwoPair,
		ThreeOfAKind,
		FullHouse,
		FourOfAKind,
		FiveOfAKind
	};

	struct Hand
	{
		Array<Card, 5> Cards;
		HandType Type = HandType::Invalid;
		u32 Bid = 0;

		HandType DetermineType()
		{
			Assert(Type == HandType::Invalid, "Type already determined, redoing it is likely unnecessary.");

			std::unordered_map<Card, u32> cardTypeCounts;
			for (const Card& card : Cards)
			{
				cardTypeCounts[card]++;
			}

			auto numDifferentCards = [&](u32 count) { return cardTypeCounts.size() == count; };

			auto amountSameCardsExists = [&](u32 count) {
					return std::any_of(cardTypeCounts.begin(), cardTypeCounts.end(),
						[&](const std::pair<Card, u32>& keyValuePair) {
							return keyValuePair.second == count;
						}
					);
				};

			if (numDifferentCards(1))
			{
				Assert(amountSameCardsExists(5));
				Assert(std::adjacent_find(Cards.begin(), Cards.end(), std::not_equal_to<>()) == Cards.end());
				Type = HandType::FiveOfAKind;
			}
			else if (numDifferentCards(2))
			{
				if (amountSameCardsExists(4))
				{
					Assert(amountSameCardsExists(1));
					Type = HandType::FourOfAKind;
				}
				else
				{
					Assert(amountSameCardsExists(3) && amountSameCardsExists(2));
					Type = HandType::FullHouse;
				}
			}
			else if (numDifferentCards(3))
			{
				if (amountSameCardsExists(3) && amountSameCardsExists(1))
				{
					Type = HandType::ThreeOfAKind;
				}
				else
				{
					Assert(amountSameCardsExists(2) && amountSameCardsExists(1));
					Type = HandType::TwoPair;
				}
			}
			else if (numDifferentCards(4))
			{
				Assert(amountSameCardsExists(2));
				Type = HandType::OnePair;
			}
			else
			{
				Assert(numDifferentCards(5) && amountSameCardsExists(1));
				Type = HandType::HighCard;
			}

			return Type;
		}

		bool operator<(const Hand& rhs) const
		{
			if (Type == rhs.Type)
			{
				for (u32 i = 0; i < Cards.size(); i++)
				{
					const Card& lhsCard = Cards[i];
					const Card& rhsCard = rhs.Cards[i];

					if (lhsCard == rhsCard)
					{
						continue;
					}

					return lhsCard < rhsCard;
				}
			}

			return Type < rhs.Type;
		}
	};

	export String ExecutePart1()
	{
		auto input = OpenInput("day7.txt");

		Vector<Hand> hands;

		String line;
		while (std::getline(input, line))
		{
			Vector<String> handAndBidString = Split(line, " ");
			const String& handString = handAndBidString[0];
			const String& bidString = handAndBidString[1];

			const u32 bid = std::stoi(bidString);

			Hand hand;
			hand.Bid = bid;
			for (u32 i = 0; i < handString.size(); i++)
			{
				const char cardChar = handString[i];
				Card& cardToWrite = hand.Cards[i];

				Card card = CharToCard(cardChar);
				Assert(card != Card::Invalid, "Unkown char for card");
				cardToWrite = card;
			}
			hand.DetermineType();
			hands.push_back(std::move(hand));
		}

		std::sort(hands.begin(), hands.end());

		u32 result = 0;
		for (u32 handIdx = 0; handIdx < hands.size(); handIdx++)
		{
			const Hand& hand = hands[handIdx];

			const u32 rank = handIdx + 1;
			const u32& bid = hand.Bid;

			result += rank * bid;
		}

		return std::to_string(result);
	}

	export String ExecutePart2()
	{
		return "";
	}
}