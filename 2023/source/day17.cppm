export module day17;

import common;

export namespace aoc
{
	enum class Direction
	{
		Invalid,
		Right,
		Down,
		Left,
		Up
	};

	Direction Opposite(Direction dir)
	{
		switch (dir)
		{
			case Direction::Right: return Direction::Left;
			case Direction::Down: return Direction::Up;
			case Direction::Left: return Direction::Right;
			case Direction::Up: return Direction::Down;
		}
		return Direction::Invalid;
	}

	struct Block
	{
		struct Link
		{
			Direction Dir = Direction::Invalid;
			Block* BlockTo = nullptr;
			u32 HeatCost = 0;
			u8 SameDirCost = 0;
			Block* BlockFrom = nullptr;

			Vector<std::pair<Vec2I, Direction>> DebugPath;

			constexpr bool operator==(const Link& other) const
			{
				return Dir == other.Dir
					&& BlockTo == other.BlockTo
					&& BlockFrom == other.BlockFrom
					&& SameDirCost == other.SameDirCost;
			}

			constexpr std::partial_ordering operator<=>(const Link& rhs) const
			{
				if (Dir != rhs.Dir
					|| BlockTo != rhs.BlockTo
					|| BlockFrom != rhs.BlockFrom)
				{
					return std::partial_ordering::unordered;
				}

				const auto heatCostOrdering = HeatCost <=> rhs.HeatCost;
				if (heatCostOrdering != 0)
				{
					return heatCostOrdering;
				}

				return SameDirCost <=> rhs.SameDirCost;
			}
		};

		Block(const Vec2I& pos, u8 heatLoss)
			: Position(pos)
			, HeatLoss(heatLoss)
		{}

		constexpr bool operator==(const Block& other) const
		{
			return Position == other.Position;
		}

		Vec2I Position;
		u8 HeatLoss;

		Vector<Link> LinksIn;
		Vector<Link> LinksOut;

		mutable Direction DebugDir = Direction::Invalid;
	};

	struct Map
	{
		Vector<Vector<Block>> Blocks;

		u32 Width() const
		{
			return NarrowSizeT(Blocks.size());
		}

		u32 Height() const
		{
			if (Blocks.empty())
			{
				return 0;
			}

			return NarrowSizeT(Blocks[0].size());
		}

		Vec2I Size() const
		{
			return Vec2I(Width(), Height());
		}

		bool WithinBounds(Vec2I pos) const
		{
			return pos.Y >= 0
				&& pos.Y < Blocks.size()
				&& pos.X >= 0
				&& pos.X < Blocks[pos.Y].size();
		}

		Block& Access(Vec2I pos)
		{
			Assert(WithinBounds(pos));
			return Blocks[pos.Y][pos.X];
		}

		// multidimensional subscript is apparently not available in msvc at this point
		/*u8& operator[](u32 x, u32 y)
		{
			Assert(y < Blocks.size()
				&& x < Blocks[y].size());
			return Blocks[y][x];
		}

		u8 operator[](u32 x, u32 y) const
		{
			Assert(y < Blocks.size()
				&& x < Blocks[y].size());
			return Blocks[y][x];
		}*/

		void DebugOutputCurrentBestPathToEnd() const
		{
			if constexpr (IsDebug() == false)
			{
				return;
			}

			Map copy = *this;

			Vec2I endPos = Size() - Vec2I(1, 1);
			Block* endBlock = &copy.Access(endPos);

			auto minHeatLink = std::ranges::min_element(endBlock->LinksIn, [](const Block::Link& lhs, const Block::Link& rhs) { return lhs.HeatCost < rhs.HeatCost; });
			if (minHeatLink != endBlock->LinksIn.end())
			{
				Block::Link link = *minHeatLink;
				for (auto pathIt = link.DebugPath.rbegin(); pathIt < link.DebugPath.rend(); ++pathIt)
				{
					const Vec2I& pos = pathIt->first;
					const Direction dir = pathIt->second;
					Block& copyBlock = copy.Access(pos);
					copyBlock.DebugDir = dir;
				}
			}

			for (const auto& row : copy.Blocks)
			{
				String rowStr;
				for (const Block& block : row)
				{
					switch (block.DebugDir)
					{
						case Direction::Right: rowStr.push_back('>'); continue;
						case Direction::Left: rowStr.push_back('<'); continue;
						case Direction::Up: rowStr.push_back('^'); continue;
						case Direction::Down: rowStr.push_back('v'); continue;
					}
					rowStr.push_back('0' + block.HeatLoss);
				}
				Log(rowStr);
			}
			Log("");
		}

		u32 FindLowestHeatLossFromCruciblePath(bool ultra = false)
		{
			// optimize neighbor iteration
			const std::pair<Direction, Vec2I> neighborOffsets[4] = {
				std::make_pair(Direction::Left, Vec2I(-1, 0)),
				std::make_pair(Direction::Right, Vec2I(1, 0)),
				std::make_pair(Direction::Up, Vec2I(0, -1)),
				std::make_pair(Direction::Down, Vec2I(0, 1))
			};

			Vec2I startPos;
			Vec2I endPos = Size() - Vec2I(1, 1);

			const Block& startBlock = Access(startPos);
			const Block& endBlock = Access(endPos);

			Block::Link startLink = {
				.Dir = Direction::Invalid,
				.BlockTo = &const_cast<Block&>(startBlock),
				.HeatCost = 0,
				.SameDirCost = 0,
				.BlockFrom = nullptr
			};

			const_cast<Block&>(startBlock).LinksIn.push_back(startLink);

			struct
			{
				bool operator()(const Block::Link& lhs, const Block::Link& rhs) const
				{
					if (lhs.HeatCost != rhs.HeatCost)
					{
						return lhs.HeatCost > rhs.HeatCost;
					}

					return lhs.SameDirCost > rhs.SameDirCost;
				}
			} compareHeuristic;

			std::priority_queue<Block::Link, Vector<Block::Link>, decltype(compareHeuristic)> openList(compareHeuristic);

			openList.push(startLink);

			// A* but it only closes according to a heuristic of both heat and stepsInSameDir, not by visited
			while (openList.empty() == false)
			{
				// lowest path costs
				Block::Link currentLink = openList.top();
				openList.pop();

				Assert(currentLink.BlockTo != nullptr);
				Block& currentBlock = *(currentLink.BlockTo);

				for (u32 i = 0; i < 4; i++) //Go through all neighbours
				{
					const auto& neighborOffset = neighborOffsets[i];
					const Direction neighborDir = neighborOffset.first;

					if (neighborDir == Opposite(currentLink.Dir)
						&& currentLink.Dir != Direction::Invalid)
					{
						// can't reverse (direction)
						continue;
					}

					const Vec2I& neighborPosOffset = neighborOffset.second;
					const Vec2I neighborPos = currentBlock.Position + neighborPosOffset;

					if (WithinBounds(neighborPos) == false)
					{
						// stay in map
						continue;
					}

					u8 sameDirCost = 1;
					if (neighborDir == currentLink.Dir || currentLink.Dir == Direction::Invalid)
					{
						// moving in same direction
						sameDirCost = currentLink.SameDirCost + 1;
					}

					if (ultra)
					{
						if (sameDirCost > 10
							|| (
								sameDirCost < 4
								&& currentLink.SameDirCost < 4 // moved at least 4 before turning
								&& neighborDir != currentLink.Dir
								&& currentLink.Dir != Direction::Invalid
								)
							)
						{
							continue;
						}
					}
					else if (sameDirCost > 3)
					{
						// dont take more than 3 steps
						continue;
					}

					Block& neighborBlock = Access(neighborPos);
					const u32 heatCost = currentLink.HeatCost + static_cast<u32>(neighborBlock.HeatLoss);

					Block::Link newLinkToNeighbor = {
						.Dir = neighborDir,
						.BlockTo = &neighborBlock,
						.HeatCost = heatCost,
						.SameDirCost = sameDirCost,
						.BlockFrom = &currentBlock
					};

					newLinkToNeighbor.DebugPath = currentLink.DebugPath;
					newLinkToNeighbor.DebugPath.emplace_back(neighborPos, neighborDir);

					bool linkAlreadyExists = false;
					for (Block::Link& link : neighborBlock.LinksIn)
					{
						if (link.Dir != newLinkToNeighbor.Dir)
						{
							if constexpr (IsDebug())
							{
								Assert(link.BlockTo == newLinkToNeighbor.BlockTo);
								Assert(link.BlockFrom != newLinkToNeighbor.BlockFrom || link.BlockFrom == nullptr);
							}

							// unrelated link
							continue;
						}

						if (link == newLinkToNeighbor)
						{
							linkAlreadyExists = true;
						}
					}

					if (linkAlreadyExists)
					{
						continue;
					}

					neighborBlock.LinksIn.push_back(newLinkToNeighbor);
					currentBlock.LinksOut.push_back(newLinkToNeighbor);
					openList.push(newLinkToNeighbor);
				}
			}

			DebugOutputCurrentBestPathToEnd();

			auto minHeatLink = std::ranges::min_element(endBlock.LinksIn, [](const Block::Link& lhs, const Block::Link& rhs) { return lhs.HeatCost < rhs.HeatCost; });
			if (minHeatLink != endBlock.LinksIn.end())
			{
				return minHeatLink->HeatCost;
			}

			// end not found
			return 0;
		}
	};

	export String ExecutePart1()
	{
		auto input = OpenInput("day17.txt");

		Map map;

		u32 y = 0;
		String line;
		while (std::getline(input, line))
		{
			Vector<Block>& row = map.Blocks.emplace_back();
			for (u32 x = 0; x < line.size(); ++x)
			{
				const char c = line[x];
				u8 heatLoss = static_cast<u8>(c - '0');
				Assert(heatLoss < 10);
				row.emplace_back(Vec2I(x, y), heatLoss);
			}
			++y;
		}

		u32 result = map.FindLowestHeatLossFromCruciblePath();

		return std::to_string(result);
	}

	export String ExecutePart2()
	{
		auto input = OpenInput("day17.txt");

		Map map;

		u32 y = 0;
		String line;
		while (std::getline(input, line))
		{
			Vector<Block>& row = map.Blocks.emplace_back();
			for (u32 x = 0; x < line.size(); ++x)
			{
				const char c = line[x];
				u8 heatLoss = static_cast<u8>(c - '0');
				Assert(heatLoss < 10);
				row.emplace_back(Vec2I(x, y), heatLoss);
			}
			++y;
		}

		u32 result = map.FindLowestHeatLossFromCruciblePath(true);

		return std::to_string(result);
	}
}