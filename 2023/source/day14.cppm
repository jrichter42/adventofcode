export module day14;

import common;

export namespace aoc
{
	enum class Cell
	{
		Invalid,
		Empty,
		RockCubed,
		RockRound
	};

	Cell CharToCell(char c)
	{
		switch (c)
		{
			case '.': return Cell::Empty;
			case '#': return Cell::RockCubed;
			case 'O': return Cell::RockRound;
			default: Assert(false);
		}
		return Cell::Invalid;
	}

	char CellToChar(Cell c)
	{
		switch (c)
		{
			case Cell::Empty: return '.';
			case Cell::RockCubed: return '#';
			case Cell::RockRound: return 'O';
			default: Assert(false);
		}
		return ' ';
	}

	enum class Direction
	{
		North,
		West,
		South,
		East,
		COUNT
	};

	Direction operator+(Direction& d, u32 count) // post increment
	{
		return static_cast<Direction>(
			(static_cast<std::underlying_type_t<Direction>>(d) + count)
			% static_cast<std::underlying_type_t<Direction>>(Direction::COUNT)
			);
	}

	struct Platform
	{
		Vector<Vector<Cell>> Rows;
		u32 CycleToReachThis = 0; // too lazy to support pair in SpinCycle caching

		bool operator==(const Platform& other) const
		{
			for (u32 y = 0; y < Rows.size(); y++)
			{
				if (Rows[y] != other.Rows[y])
				{
					return false;
				}
			}
			return true;
		}

		void Tilt(Direction dir)
		{
			while (true)
			{
				bool rocksRolled = false;
				for (u32 y = 0; y < Rows.size(); y++)
				{
					Vector<Cell>& row = Rows[y];
					for (u32 x = 0; x < row.size(); x++)
					{
						Cell* cellInDirectionPtr = [&]() -> Cell* {
							switch (dir)
							{
								case Direction::North:
								{
									if (y == 0)
									{
										return nullptr;
									}
									return &Rows[y - 1][x];
								} break;
								case Direction::East:
								{
									if (x == row.size() - 1)
									{
										return nullptr;
									}
									return &Rows[y][x + 1];
								} break;
								case Direction::West:
								{
									if (x == 0)
									{
										return nullptr;
									}
									return &Rows[y][x - 1];
								} break;
								case Direction::South:
								{
									if (y == Rows.size() - 1)
									{
										return nullptr;
									}
									return &Rows[y + 1][x];
								} break;
							}
							return nullptr;
						}();

						if (cellInDirectionPtr == nullptr)
						{
							continue;
						}

						Cell& cell = row[x];
						Cell& cellInDirection = *cellInDirectionPtr;
						if (cell == Cell::RockRound
							&& cellInDirection == Cell::Empty)
						{
							cellInDirection = Cell::RockRound;
							cell = Cell::Empty;
							rocksRolled = true;
						}
					}
				}
				if (!rocksRolled)
				{
					break;
				}
			}
		}

		// returns num of cycles that can be skipped due to loop
		u32 SpinCycle(u32 cycle)
		{
			Direction dir = Direction::North;
			for (u32 i = 0; i < 4; i++)
			{
				Tilt(dir);
				dir = dir + 1;
			}
			Assert(dir == Direction::North);

			cycle++;

			static Vector<Platform> CachedPlatformStates;

			auto cacheIt = std::find(CachedPlatformStates.begin(), CachedPlatformStates.end(), *this);
			if (cacheIt != CachedPlatformStates.end())
			{
				return cycle - cacheIt->CycleToReachThis;
			}

			CycleToReachThis = cycle;
			CachedPlatformStates.push_back(*this);
			return 0;
		}

		u64 CalculateWeightNorth()
		{
			u64 weightSum = 0;
			const u32 numRows = NarrowSizeT(Rows.size());
			for (u32 y = 0; y < numRows; y++)
			{
				Vector<Cell>& row = Rows[y];
				for (u32 x = 0; x < row.size(); x++)
				{
					if (row[x] == Cell::RockRound)
					{
						weightSum += numRows - y;
					}
				}
			}

			return weightSum;
		}
	};

	export String ExecutePart1()
	{
		auto input = OpenInput("day14.txt");

		Platform platform;

		String line;
		while (std::getline(input, line))
		{
			Vector<Cell> row;
			for (char c : line)
			{
				row.push_back(CharToCell(c));
			}
			platform.Rows.push_back(std::move(row));
		}

		platform.Tilt(Direction::North);

		u64 result = platform.CalculateWeightNorth();

		return std::to_string(result);
	}

	export String ExecutePart2()
	{
		auto input = OpenInput("day14.txt");

		Platform platform;

		String line;
		while (std::getline(input, line))
		{
			Vector<Cell> row;
			for (char c : line)
			{
				row.push_back(CharToCell(c));
			}
			platform.Rows.push_back(std::move(row));
		}

		for (u32 i = 0; i < 1000000000; i++)
		{
			if (i % 100000 == 0)
			{
				LogFormat("{}", i/1000000000.f);
			}

			const u32 cyclesToSkip = platform.SpinCycle(i);
			if (cyclesToSkip > 0)
			{
				// loop found
				if (i < 1000000000 - cyclesToSkip) { LogFormat("skipping in steps of {}", cyclesToSkip); }
				while(i < 1000000000 - cyclesToSkip)
				{
					i += cyclesToSkip;
				}
			}
		}

		u64 result = platform.CalculateWeightNorth();

		return std::to_string(result);
	}
}