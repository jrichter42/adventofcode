export module day21;

import common;

export namespace aoc
{
	using Vec2I = Vec2T<s64>;
	enum class Direction
	{
		Invalid,
		Right,
		Down,
		Left,
		Up
	};

	constexpr Vec2I DirToVec2(Direction dir)
	{
		switch (dir)
		{
			case Direction::Right: return Vec2I(1, 0);
			case Direction::Down: return Vec2I(0, 1);
			case Direction::Left: return Vec2I(-1, 0);
			case Direction::Up: return Vec2I(0, -1);
		}
		return Vec2I();
	}

	enum class Tile
	{
		Invalid,
		GardenClosed,
		GardenOpen,
		Rock
	};

	export String ExecutePart1()
	{
		auto input = OpenInput("day21.txt");

		Array<Array<Tile, 131>, 131> ground;
		Vec2I groundDimensions;
		Vec2I startPos;

		auto debugOutputGround = [&]() {
			if constexpr (IsDebug() == false)
			{
				return;
			}
			Log("");
			for (u32 y = 0; y < groundDimensions.Y; ++y)
			{
				String row;
				for (u32 x = 0; x < groundDimensions.X; ++x)
				{
					if (x == startPos.X && y == startPos.Y)
					{
						row.push_back('S');
						continue;
					}
					switch (ground[y][x])
					{
						case Tile::GardenClosed:	row.push_back('O'); break;
						case Tile::GardenOpen:		row.push_back('.'); break;
						case Tile::Rock:			row.push_back('#'); break;
					}
				}
				Log(row);
			}
			Log("");
		};

		u32 y = 0;
		String line;
		while (std::getline(input, line))
		{
			Assert(y < ground.size());
			auto& row = ground[y];
			Assert(line.size() <= row.size());

			groundDimensions = Vec2I(line.size(), y + 1);

			row.fill(Tile::Invalid);
			for (u32 x = 0; x < line.size(); ++x)
			{
				const char c = line[x];
				if (c == 'S')
				{
					startPos = Vec2I(x, y);
					row[x] = Tile::GardenClosed;
				}
				else if (c == '.')
				{
					row[x] = Tile::GardenOpen;
				}
				else if (c == '#')
				{
					row[x] = Tile::Rock;
				}
			}
			++y;
		}

		// flood fill/walk until steps exhausted
		constexpr bool findPossibleEndPosInstead = true;
		u32 remainingSteps = 64;

		std::queue<Vec2I> floodFillCurrentHeads;
		std::queue<Vec2I> floodFillNextHeads;
		Vec2I pos;

		floodFillCurrentHeads.push(startPos);

		u64 reachablePlots = 1;

		auto tryStep = [&](const Vec2I& currentPos, Direction dir) {
			Vec2I pos = currentPos + DirToVec2(dir);
			if (pos.X >= 0
				&& pos.Y >= 0
				&& pos.X < groundDimensions.X
				&& pos.Y < groundDimensions.Y)
			{
				if (ground[pos.Y][pos.X] == Tile::GardenOpen)
				{
					floodFillNextHeads.push(pos);
					ground[pos.Y][pos.X] = Tile::GardenClosed;
					++reachablePlots;
				}
				else if (( ground[pos.Y][pos.X] == Tile::GardenOpen
						 || ground[pos.Y][pos.X] == Tile::GardenClosed )
						 && findPossibleEndPosInstead)
				{
					floodFillNextHeads.push(pos);
				}
			}
		};

		debugOutputGround();

		while (floodFillCurrentHeads.empty() == false && remainingSteps > 0)
		{
			if constexpr (IsDebug() || false)
			{
				LogFormat("heads: {} remaining steps: {}", floodFillCurrentHeads.size(), remainingSteps);
			}

			while (floodFillCurrentHeads.empty() == false)
			{
				const Vec2I& currentHead = floodFillCurrentHeads.front();
				floodFillCurrentHeads.pop();

				tryStep(currentHead, Direction::Left);
				tryStep(currentHead, Direction::Up);
				tryStep(currentHead, Direction::Right);
				tryStep(currentHead, Direction::Down);
			}

			debugOutputGround();

			--remainingSteps;
			floodFillCurrentHeads.swap(floodFillNextHeads);

			if constexpr (findPossibleEndPosInstead)
			{
				// deduplicate queue, hacky but works
				Vector<Vec2I> headPositions;
				while (floodFillCurrentHeads.empty() == false)
				{
					headPositions.push_back(floodFillCurrentHeads.front());
					floodFillCurrentHeads.pop();
				}

				std::sort(headPositions.begin(), headPositions.end());
				headPositions.erase(std::unique(headPositions.begin(), headPositions.end()), headPositions.end());

				for (auto& endPos : headPositions)
				{
					floodFillCurrentHeads.push(endPos);
				}
			}
		}

		if constexpr (findPossibleEndPosInstead)
		{
			Vector<Vec2I> endPositions;
			while (floodFillCurrentHeads.empty() == false)
			{
				endPositions.push_back(floodFillCurrentHeads.front());
				floodFillCurrentHeads.pop();
			}

			std::sort(endPositions.begin(), endPositions.end());
			endPositions.erase(std::unique(endPositions.begin(), endPositions.end()), endPositions.end());
			const u64 exactStepsReachability = endPositions.size();
			return std::to_string(exactStepsReachability);
		}
		//return std::to_string(reachablePlots);
	}

	export String ExecutePart2()
	{
		auto input = OpenInput("day21.txt");

		Array<Array<Tile, 131>, 131> ground;
		Vec2I groundDimensions;
		Vec2I startPos;

		auto debugOutputGround = [&]() {
			if constexpr (IsDebug() == false)
			{
				return;
			}
			Log("");
			for (u32 y = 0; y < groundDimensions.Y; ++y)
			{
				String row;
				for (u32 x = 0; x < groundDimensions.X; ++x)
				{
					if (x == startPos.X && y == startPos.Y)
					{
						row.push_back('S');
						continue;
					}
					switch (ground[y][x])
					{
						case Tile::GardenClosed:	row.push_back('O'); break;
						case Tile::GardenOpen:		row.push_back('.'); break;
						case Tile::Rock:			row.push_back('#'); break;
					}
				}
				Log(row);
			}
			Log("");
		};

		u32 y = 0;
		String line;
		while (std::getline(input, line))
		{
			Assert(y < ground.size());
			auto& row = ground[y];
			Assert(line.size() <= row.size());

			groundDimensions = Vec2I(line.size(), y + 1);

			row.fill(Tile::Invalid);
			for (u32 x = 0; x < line.size(); ++x)
			{
				const char c = line[x];
				if (c == 'S')
				{
					startPos = Vec2I(x, y);
					row[x] = Tile::GardenClosed;
				}
				else if (c == '.')
				{
					row[x] = Tile::GardenOpen;
				}
				else if (c == '#')
				{
					row[x] = Tile::Rock;
				}
			}
			++y;
		}

		// flood fill/walk until steps exhausted
		constexpr bool findPossibleEndPosInstead = true;
		constexpr u32 stepsNeeded = 26501365;

		u32 remainingSteps = stepsNeeded;

		std::queue<Vec2I> floodFillCurrentHeads;
		std::queue<Vec2I> floodFillNextHeads;
		Vec2I pos;

		floodFillCurrentHeads.push(startPos);

		//u64 reachablePlots = 1;

		auto tryStep = [&](const Vec2I& currentPos, Direction dir) {
			Vec2I pos = currentPos + DirToVec2(dir);
			if (pos.X >= 0
				&& pos.Y >= 0)
			{
				Vec2I groundAccessPos(
					pos.X % groundDimensions.X,
					pos.Y = pos.Y % groundDimensions.Y);

				if (ground[groundAccessPos.Y][groundAccessPos.X] == Tile::GardenOpen)
				{
					floodFillNextHeads.push(pos);
					// doesnt work with this access pattern but not needed rn
					//ground[groundAccessPos.Y][groundAccessPos.X] = Tile::GardenClosed;
					//++reachablePlots;
				}
				else if (( ground[groundAccessPos.Y][groundAccessPos.X] == Tile::GardenOpen
						  || ground[groundAccessPos.Y][groundAccessPos.X] == Tile::GardenClosed )
						 && findPossibleEndPosInstead)
				{
					floodFillNextHeads.push(pos);
				}
			}
		};

		debugOutputGround();

		struct Area
		{
			// should build rect type
			Vec2I PosMin;
			Vec2I PosMax;

		};

		Vector<std::pair<u64, u64>> resultAtStep{ std::make_pair(0, 1) };
		resultAtStep.reserve(u16_MAX);

		while (floodFillCurrentHeads.empty() == false && remainingSteps > 0)
		{
			if constexpr (IsDebug() || false)
			{
				//LogFormat("heads: {} remaining steps: {}", floodFillCurrentHeads.size(), remainingSteps);
			}

			while (floodFillCurrentHeads.empty() == false)
			{
				const Vec2I& currentHead = floodFillCurrentHeads.front();
				floodFillCurrentHeads.pop();

				tryStep(currentHead, Direction::Left);
				tryStep(currentHead, Direction::Up);
				tryStep(currentHead, Direction::Right);
				tryStep(currentHead, Direction::Down);
			}

			//debugOutputGround();

			--remainingSteps;
			floodFillCurrentHeads.swap(floodFillNextHeads);

			if constexpr (findPossibleEndPosInstead)
			{
				/*if (floodFillCurrentHeads.size() < (s32_MAX / 64))
				{
					// don't overdo it
					continue;
				}

				LogFormat("(cleaning) heads: {} remaining steps: {}", floodFillCurrentHeads.size(), remainingSteps);*/

				// deduplicate queue, hacky but works
				Vector<Vec2I> headPositions;
				headPositions.reserve(floodFillCurrentHeads.size());
				while (floodFillCurrentHeads.empty() == false)
				{
					Vec2I& front = floodFillCurrentHeads.front();
					//headPositions.push_back(floodFillCurrentHeads.front());
					auto bounds = std::ranges::equal_range(headPositions, front);
					if (bounds.begin() == bounds.end())
					{
						// new unique element
						headPositions.insert(bounds.begin(), front);
					}
					floodFillCurrentHeads.pop();
				}

				//std::sort(headPositions.begin(), headPositions.end());
				//headPositions.erase(std::unique(headPositions.begin(), headPositions.end()), headPositions.end());

				for (auto& endPos : headPositions)
				{
					floodFillCurrentHeads.push(endPos);
				}

				const u64 step = stepsNeeded - remainingSteps;
				auto& stepPair = resultAtStep.emplace_back(step, floodFillCurrentHeads.size());

				if (step < 100 || step % 2 == 1)
				{
					// let it ramp up and make results more stable
					continue;
				}

				const u64 pastStep = step / 2;
				auto& pastStepPair = resultAtStep[pastStep];

				const Vec2I direction(stepPair.first - pastStepPair.first, stepPair.second - pastStepPair.second);

				const u64 stepsDeltaCount = step - pastStep;
				Assert(ToUnsigned(direction.X) == stepsDeltaCount);
				const u64 stepsFromPastToEnd = stepsNeeded - pastStep;
				const f64 dirScale = f64(stepsFromPastToEnd) / f64(stepsDeltaCount);

				Vec2I extrapolatedEnd = direction * dirScale;
				const u64 extrapolatedResult = extrapolatedEnd.Y + pastStep;

				LogFormat("{} ({}..{}..)", extrapolatedResult, pastStep, step);
				//LogFormat("(cleaned) heads: {} remaining steps: {}", remainingSteps, floodFillCurrentHeads.size(), remainingSteps);
			}
		}

		if constexpr (findPossibleEndPosInstead)
		{
			/*Vector<Vec2I> endPositions;
			endPositions.reserve(floodFillCurrentHeads.size());
			while (floodFillCurrentHeads.empty() == false)
			{
				Vec2I& front = floodFillCurrentHeads.front();
				auto bounds = std::ranges::equal_range(endPositions, front);
				if (bounds.begin() == bounds.end())
				{
					// new unique element
					endPositions.insert(bounds.begin(), front);
				}
				floodFillCurrentHeads.pop();
			}
			//std::sort(endPositions.begin(), endPositions.end());
			//endPositions.erase(std::unique(endPositions.begin(), endPositions.end()), endPositions.end());*/

			const u64 exactStepsReachability = floodFillCurrentHeads.size();
			return std::to_string(exactStepsReachability);
		}
		//return std::to_string(reachablePlots);
	}
}