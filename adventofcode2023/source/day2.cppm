export module day2;

import common;

export namespace aoc
{
	struct CubeSet
	{
		u32 Red = 0;
		u32 Green = 0;
		u32 Blue = 0;

		bool Validate(const CubeSet& candidate) const
		{
			if (candidate.Red > Red
				|| candidate.Green > Green
				|| candidate.Blue > Blue)
			{
				return false;
			}

			return true;
		}
	};

	vector<String> Split(const String& string, const String& delimiterStr)
	{
		vector<String> result;

		size_t searchPos = 0;
		while (searchPos < string.size())
		{
			size_t occurence = string.find(delimiterStr, searchPos);
			if (occurence == String::npos)
			{
				if (searchPos < (string.size() - delimiterStr.size()))
				{
					result.push_back(string.substr(searchPos));
				}
				break;
			}
			result.push_back(string.substr(searchPos, occurence - searchPos));
			searchPos = occurence + delimiterStr.size();
		}

		return result;
	}

	vector<String> Split(const String& string, const char delimiterChar)
	{
		return Split(string, String(1, delimiterChar));
	}

	export String ExecutePart1()
	{
		const CubeSet bag = { .Red = 12, .Green = 13, .Blue = 14 };

		auto input = OpenInput("day2.txt");

		u32 validGameIDSum = 0;

		for (String line; std::getline(input, line);)
		{
			CubeSet drawnCubes;
			vector<String> game = Split(line, ": ");

			String gameIDStr = game[0].substr(strlen("Game "));
			s32 gameIDSigned = stoi(gameIDStr);
			Assert(gameIDSigned >= 0);
			u32 gameID = static_cast<u32>(gameIDSigned);

			vector<String> draws = Split(game[1], "; ");
			std::for_each(draws.begin(), draws.end(),
				[&](const String& draw)
				{
					vector<String> drawColorCounts = Split(draw, ", ");
					std::for_each(drawColorCounts.begin(), drawColorCounts.end(),
						[&](const String& drawColorCount)
						{
							vector<String> nrColorPair = Split(drawColorCount, " ");

							s32 countSigned = stoi(nrColorPair[0]);
							Assert(countSigned >= 0);
							u32 count = static_cast<u32>(countSigned);

							const String& color = nrColorPair[1];
							if (strcmp(color.c_str(), "red") == 0)
							{
								drawnCubes.Red = std::max(drawnCubes.Red, count);
							}
							else if (strcmp(color.c_str(), "green") == 0)
							{
								drawnCubes.Green = std::max(drawnCubes.Green, count);
							}
							else if (strcmp(color.c_str(), "blue") == 0)
							{
								drawnCubes.Blue = std::max(drawnCubes.Blue, count);
							}
							else
							{
								Assert(false);
							}
						});
				});

			if (bag.Validate(drawnCubes))
			{
				validGameIDSum += gameID;
			}
		}

		return std::to_string(validGameIDSum);
	}

	export String ExecutePart2()
	{
		return "";
	}
}