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

	CubeSet CalculateCubesNeededForGame(const String& drawsStr)
	{
		CubeSet result;

		Vector<String> draws = Split(drawsStr, "; ");
		std::for_each(draws.begin(), draws.end(),
			[&](const String& draw)
			{
				Vector<String> drawColorCounts = Split(draw, ", ");
				std::for_each(drawColorCounts.begin(), drawColorCounts.end(),
					[&](const String& drawColorCount)
					{
						Vector<String> nrColorPair = Split(drawColorCount, " ");

						u32 count = ToUnsigned(stoi(nrColorPair[0]));

						const String& color = nrColorPair[1];
						if (color == "red")
						{
							result.Red = std::max(result.Red, count);
						}
						else if (color == "green")
						{
							result.Green = std::max(result.Green, count);
						}
						else if (color == "blue")
						{
							result.Blue = std::max(result.Blue, count);
						}
						else
						{
							Assert(false);
						}
					});
			});

		return result;
	}

	export String ExecutePart1()
	{
		const CubeSet bag = { .Red = 12, .Green = 13, .Blue = 14 };

		auto input = OpenInput("day2.txt");

		u32 validGameIDSum = 0;

		for (String line; std::getline(input, line);)
		{
			Vector<String> game = Split(line, ": ");

			String gameIDStr = game[0].substr(std::strlen("Game "));
			u32 gameID = ToUnsigned(stoi(gameIDStr));

			CubeSet drawnCubes = CalculateCubesNeededForGame(game[1]);

			if (bag.Validate(drawnCubes))
			{
				validGameIDSum += gameID;
			}
		}

		return std::to_string(validGameIDSum);
	}

	export String ExecutePart2()
	{
		const CubeSet bag = { .Red = 12, .Green = 13, .Blue = 14 };

		auto input = OpenInput("day2.txt");

		u32 powerSum = 0;

		for (String line; std::getline(input, line);)
		{
			Vector<String> game = Split(line, ": ");

			CubeSet drawnCubes = CalculateCubesNeededForGame(game[1]);

			u32 gamePowerSum = drawnCubes.Red * drawnCubes.Green * drawnCubes.Blue;
			powerSum += gamePowerSum;
		}

		return std::to_string(powerSum);
	}
}