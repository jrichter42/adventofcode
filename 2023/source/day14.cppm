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

	struct Platform
	{
		Vector<Vector<Cell>> Rows;

		void TiltNorth()
		{
			while (true)
			{
				bool rocksRolled = false;
				for (u32 y = 1; y < Rows.size(); y++)
				{
					Vector<Cell>& rowNorth = Rows[y - 1];
					Vector<Cell>& row = Rows[y];
					for (u32 x = 0; x < row.size(); x++)
					{
						if (row[x] == Cell::RockRound
							&& rowNorth[x] == Cell::Empty)
						{
							rowNorth[x] = Cell::RockRound;
							row[x] = Cell::Empty;
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

		platform.TiltNorth();

		u64 result = platform.CalculateWeightNorth();

		return std::to_string(result);
	}

	export String ExecutePart2()
	{
		return "";
	}
}