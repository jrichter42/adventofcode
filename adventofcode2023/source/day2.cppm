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

	export String ExecutePart1()
	{
		const CubeSet bag = { .Red = 12, .Green = 13, .Blue = 14 };

		auto input = OpenInput("day2.txt");

		u32 validGameIDSum = 0;

		for (String line; std::getline(input, line);)
		{
			CubeSet drawnCubes;
			Vector<String> game = Split(line, ": ");

			String gameIDStr = game[0].substr(strlen("Game "));
			u32 gameID = ToUnsigned(stoi(gameIDStr));

			Vector<String> draws = Split(game[1], "; ");
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
		const CubeSet bag = { .Red = 12, .Green = 13, .Blue = 14 };

		auto input = OpenInput("day2.txt");

		u32 powerSum = 0;

		for (String line; std::getline(input, line);)
		{
			CubeSet drawnCubes;
			Vector<String> game = Split(line, ": ");

			Vector<String> draws = Split(game[1], "; ");
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

			u32 gamePowerSum = drawnCubes.Red * drawnCubes.Green * drawnCubes.Blue;
			powerSum += gamePowerSum;
		}

		return std::to_string(powerSum);
	}
}