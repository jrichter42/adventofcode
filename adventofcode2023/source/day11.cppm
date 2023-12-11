export module day11;

import common;

export namespace aoc
{
	struct Cell
	{
		static constexpr u32 GalaxyID_Invalid = 0;
		static inline u32 GalaxyID_Next = 1;

		Cell() : Cell(false) {}

		Cell(bool isGalaxy)
			: IsGalaxy(isGalaxy)
			, GalaxyID(isGalaxy ? GalaxyID_Next++ : GalaxyID_Invalid)
		{}

		bool operator==(const Cell& other) const
		{
			if (other.IsGalaxy && IsGalaxy)
			{
				return GalaxyID == other.GalaxyID;
			}
			return IsGalaxy == other.IsGalaxy;
		}

		bool operator==(char character) const
		{
			if (character == '#')
			{
				return IsGalaxy;
			}
			else if (character == '.')
			{
				return !IsGalaxy;
			}
			return false;
		}

		bool IsGalaxy = false;
		u32 GalaxyID = GalaxyID_Invalid;
		u32 PosX = 0;
		u32 PosY = 0;

		u64 PosXExpanded(Vector<u32> expandedColumns) const
		{
			u64 result = PosX;
			for (u32 expandedColumn : expandedColumns)
			{
				if (expandedColumn < PosX)
				{
					result += 1000000 - 1;
				}
			}
			return result;
		}

		u64 PosYExpanded(Vector<u32> expandedRows) const
		{
			u64 result = PosY;
			for (u32 expandedRow : expandedRows)
			{
				if (expandedRow < PosY)
				{
					result += 1000000 - 1;
				}
			}
			return result;
		}
	};

	export String ExecutePart1()
	{
		auto input = OpenInput("day11.txt");

		Vector<Vector<Cell>> map;

		String line;
		while (std::getline(input, line))
		{
			Vector<Cell>& row = map.emplace_back();
			for (const char c : line)
			{
				if (c == '#')
				{
					row.emplace_back(true);
					continue;
				}
				Assert(c == '.');
				row.emplace_back(false);
			}
		}

		const u32 rowsCount = NarrowSizeT(map.size());
		const u32 columnsCount = NarrowSizeT(map[0].size());

		// collect rows
		Vector<u32> rowsToExpand;
		for (u32 y = 0; y < map.size(); y++)
		{
			const Vector<Cell>& row = map[y];

			auto it = std::find(row.begin(), row.end(), '#');
			if (it != row.end())
			{
				continue;
			}

			rowsToExpand.push_back(y);
		}

		// collect columns
		Vector<u32> columnsToExpand;
		for (u32 x = 0; x < columnsCount; x++)
		{
			bool foundGalaxy = false;
			for (u32 y = 0; y < map.size(); y++)
			{
				const Vector<Cell>& row = map[y];
				Assert(row.size() == columnsCount);
				const Cell& cell = row[x];

				if (cell == '#')
				{
					foundGalaxy = true;
					break;
				}
			}

			if (foundGalaxy == false)
			{
				columnsToExpand.push_back(x);
			}
		}

		// expand columns
		for (auto rIt = columnsToExpand.rbegin(); rIt < columnsToExpand.rend(); rIt++)
		{
			const u32 columnToExpand = *rIt;
			for (Vector<Cell>& row : map)
			{
				row.insert(row.begin() + columnToExpand, Cell(false));
			}
		}

		const  u32 expandedColumnsCount = columnsCount + NarrowSizeT(columnsToExpand.size());
		Assert(map[0].size() == expandedColumnsCount);

		// expand rows
		for (auto rIt = rowsToExpand.rbegin(); rIt < rowsToExpand.rend(); rIt++)
		{
			const u32 rowToExpand = *rIt;
			Vector<Cell> newRow(expandedColumnsCount);
			map.insert(map.begin() + rowToExpand, std::move(newRow));
		}

		const u32 expandedRowsCount = rowsCount + NarrowSizeT(rowsToExpand.size());
		Assert(map.size() == expandedRowsCount);

		Vector<Cell*> galaxies;
		for (u32 y = 0; y < expandedRowsCount; y++)
		{
			Vector<Cell>& row = map[y];
			for (u32 x = 0; x < expandedColumnsCount; x++)
			{
				Cell& cell = row[x];
				if (cell.IsGalaxy)
				{
					cell.PosX = x;
					cell.PosY = y;
					galaxies.push_back(&cell);
				}
			}
		}

		u32 distanceStepsSum = 0;
		for (auto firstIt = galaxies.begin(); firstIt < galaxies.end(); firstIt++)
		{
			for (auto secondIt = firstIt + 1; secondIt < galaxies.end(); secondIt++)
			{
				const Cell* lhs = *firstIt;
				const Cell* rhs = *secondIt;
				const u32 distanceX = (u32)std::abs((s32)lhs->PosX - (s32)rhs->PosX);
				const u32 distanceY = (u32)std::abs((s32)lhs->PosY - (s32)rhs->PosY);
				const u32 distanceSteps = distanceX + distanceY;
				distanceStepsSum += distanceSteps;
			}
		}

		return std::to_string(distanceStepsSum);
	}

	export String ExecutePart2()
	{
		auto input = OpenInput("day11.txt");

		Vector<Vector<Cell>> map;

		String line;
		while (std::getline(input, line))
		{
			Vector<Cell>& row = map.emplace_back();
			for (const char c : line)
			{
				if (c == '#')
				{
					row.emplace_back(true);
					continue;
				}
				Assert(c == '.');
				row.emplace_back(false);
			}
		}

		const u32 rowsCount = NarrowSizeT(map.size());
		const u32 columnsCount = NarrowSizeT(map[0].size());

		// collect rows
		Vector<u32> rowsToExpand;
		for (u32 y = 0; y < map.size(); y++)
		{
			const Vector<Cell>& row = map[y];

			auto it = std::find(row.begin(), row.end(), '#');
			if (it != row.end())
			{
				continue;
			}

			rowsToExpand.push_back(y);
		}

		// collect columns
		Vector<u32> columnsToExpand;
		for (u32 x = 0; x < columnsCount; x++)
		{
			bool foundGalaxy = false;
			for (u32 y = 0; y < map.size(); y++)
			{
				const Vector<Cell>& row = map[y];
				Assert(row.size() == columnsCount);
				const Cell& cell = row[x];

				if (cell == '#')
				{
					foundGalaxy = true;
					break;
				}
			}

			if (foundGalaxy == false)
			{
				columnsToExpand.push_back(x);
			}
		}

		Vector<Cell*> galaxies;
		for (u32 y = 0; y < rowsCount; y++)
		{
			Vector<Cell>& row = map[y];
			for (u32 x = 0; x < columnsCount; x++)
			{
				Cell& cell = row[x];
				if (cell.IsGalaxy)
				{
					cell.PosX = x;
					cell.PosY = y;
					galaxies.push_back(&cell);
				}
			}
		}

		u64 distanceStepsSum = 0;
		for (auto firstIt = galaxies.begin(); firstIt < galaxies.end(); firstIt++)
		{
			for (auto secondIt = firstIt + 1; secondIt < galaxies.end(); secondIt++)
			{
				const Cell* lhs = *firstIt;
				const Cell* rhs = *secondIt;
				const u64 distanceX = (u32)std::abs((s32)lhs->PosXExpanded(columnsToExpand) - (s32)rhs->PosXExpanded(columnsToExpand));
				const u64 distanceY = (u32)std::abs((s32)lhs->PosYExpanded(rowsToExpand) - (s32)rhs->PosYExpanded(rowsToExpand));
				const u64 distanceSteps = distanceX + distanceY;
				distanceStepsSum += distanceSteps;
			}
		}

		return std::to_string(distanceStepsSum);
	}
}