export module day10;

import common;

export namespace aoc
{
	enum class CellContent
	{
		Invalid,
		Ground,
		PipeVertical,
		PipeHorizontal,
		PipeNorthEastL,
		PipeNorthWestJ,
		PipeSouthWest7,
		PipeSouthEastF,
		PipeStart
	};

	enum class Direction
	{
		Invalid,
		North,
		East,
		West,
		South
	};

	Direction OppositeDirection(Direction dir)
	{
		switch (dir)
		{
			case Direction::North: return Direction::South;
			case Direction::East: return Direction::West;
			case Direction::West: return Direction::East;
			case Direction::South: return Direction::North;
		}
		return Direction::Invalid;
	}

	bool HasPipeConnection(CellContent content, Direction dir)
	{
		if (content == CellContent::PipeStart)
		{
			return true;
		}

		switch (dir)
		{
			case Direction::North:
			{
				return content == CellContent::PipeVertical
					|| content == CellContent::PipeNorthEastL
					|| content == CellContent::PipeNorthWestJ;
			} break;
			case Direction::East:
			{
				return content == CellContent::PipeHorizontal
					|| content == CellContent::PipeNorthEastL
					|| content == CellContent::PipeSouthEastF;
			} break;
			case Direction::West:
			{
				return content == CellContent::PipeHorizontal
					|| content == CellContent::PipeNorthWestJ
					|| content == CellContent::PipeSouthWest7;
			} break;
			case Direction::South:
			{
				return content == CellContent::PipeVertical
					|| content == CellContent::PipeSouthEastF
					|| content == CellContent::PipeSouthWest7;
			} break;
			default:
			{
				Assert(false, "Invalid direction");
			} break;
		}

		return false;
	}

	struct vec2
	{
		vec2() = default;

		vec2(s32 x, s32 y)
			: X(x)
			, Y(y)
		{}

		bool operator==(const vec2& other) const
		{
			return X == other.X && Y == other.Y;
		}

		bool operator!=(const vec2& other) const
		{
			return !operator==(other);
		}

		s32 X = 0;
		s32 Y = 0;
	};

	vec2 DirectionToVec2(Direction dir)
	{
		switch (dir)
		{
		case Direction::North: return vec2(0, -1);
		case Direction::East: return vec2(1, 0);
		case Direction::West: return vec2(-1, 0);
		case Direction::South: return vec2(0, 1);
		}
		return vec2();
	}

	template<u32 SizeX, u32 SizeY>
	struct Map : protected Array<Array<CellContent, SizeX>, SizeY>
	{
		using Base = Array<Array<CellContent, SizeX>, SizeY>;

		CellContent operator[](const vec2& position) const
		{
			// underlying is inverted, reasoning: see in this file below
			const auto& row = Base::operator[](position.Y);
			return row[position.X];
		}

		Base& UnderlyingData() { return *this; }
	};

	struct DiscoveredCell
	{
		static constexpr u32 s_GlobalHorizontalSize = 140;
		static constexpr u32 s_GlobalVerticalSize = 140;
		using GlobalMap = Map<s_GlobalHorizontalSize, s_GlobalVerticalSize>;

		static constexpr u32 s_GlobalHorizontalLimit = s_GlobalHorizontalSize - 1;
		static constexpr u32 s_GlobalVerticalLimit = s_GlobalVerticalSize - 1;

		DiscoveredCell(const GlobalMap& globalMap, vec2 globalPos)
			: GlobalMapRef(const_cast<GlobalMap&>(globalMap))
			, Position(globalPos)
		{
			Content = Peek(vec2());
			Assert(Content != CellContent::Invalid);
		}

		DiscoveredCell(const DiscoveredCell& other)
			: GlobalMapRef(other.GlobalMapRef)
			, Position(other.Position)
			, Content(other.Content)
			, DiscoveredFromLocalPosition(other.DiscoveredFromLocalPosition)
			, StepsTaken(other.StepsTaken)
		{}

		DiscoveredCell& operator=(const DiscoveredCell& other)
		{
			GlobalMapRef = other.GlobalMapRef;
			Position = other.Position;
			Content = other.Content;
			DiscoveredFromLocalPosition = other.DiscoveredFromLocalPosition;
			StepsTaken = other.StepsTaken;
			return *this;
		}

		bool operator==(const DiscoveredCell& other) const
		{
			const bool result =
				&GlobalMapRef == &other.GlobalMapRef
				&& Position == other.Position;

			Assert(result == false || Content == other.Content);
			return result;
		}

		vec2 Position;
		CellContent Content = CellContent::Invalid;
		vec2 DiscoveredFromLocalPosition;
		u32 StepsTaken = 0;
	protected:
		GlobalMap& GlobalMapRef; // should be const but c++
	public:

		CellContent Peek(const vec2& localPos, bool localOnly = false) const
		{
			const vec2 localLimitBottom = vec2(
				Position.X == 0 ? 0 : Position.X - 1,
				Position.Y == 0 ? 0 : Position.Y - 1);

			const vec2 localLimitTop = vec2(
				Position.X == s_GlobalHorizontalLimit ? s_GlobalHorizontalLimit : Position.X + 1,
				Position.Y == s_GlobalVerticalLimit ? s_GlobalVerticalLimit : Position.Y + 1);

			if (localOnly && (
				localPos.X < localLimitBottom.X
				|| localPos.X > localLimitTop.X
				|| localPos.Y < localLimitBottom.Y
				|| localPos.Y > localLimitTop.Y
				))
			{
				return CellContent::Invalid;
			}

			const vec2 globalPos = vec2(
				Position.X + localPos.X,
				Position.Y + localPos.Y);

			return GlobalMapRef[globalPos];
		}

		DiscoveredCell Discover(const vec2& localPos) const
		{
			const vec2 globalPos(Position.X + localPos.X, Position.Y + localPos.Y);

			DiscoveredCell result(GlobalMapRef, globalPos);
			result.DiscoveredFromLocalPosition.X = -localPos.X;
			result.DiscoveredFromLocalPosition.Y = -localPos.Y;
			result.StepsTaken = StepsTaken + 1;

			return result;
		}

		template<typename Func>
		void ForEachConnectedNeighbor(Func&& f) const
		{
			auto invokeIfConnected = [&](Direction direction)
			{
				if (HasPipeConnection(Content, direction) == false)
				{
					return;
				}

				const vec2 localPos = DirectionToVec2(direction);
				const CellContent targetContent = Peek(localPos);
				const Direction dirTargetToSelf = OppositeDirection(direction);
				if (HasPipeConnection(targetContent, dirTargetToSelf))
				{
					f(direction, localPos, targetContent);
				}
			};

			invokeIfConnected(Direction::North);
			invokeIfConnected(Direction::East);
			invokeIfConnected(Direction::South);
			invokeIfConnected(Direction::West);
		}

		// returns branched navigators

		void DiscoverNeighbors(Vector<DiscoveredCell>& newDiscoveries) const
		{
			Vector<vec2> connectedNeighbors;
			ForEachConnectedNeighbor([&](const Direction direction, const vec2& localPos, const CellContent cellContent) {
				if (cellContent == CellContent::Invalid
					|| localPos == DiscoveredFromLocalPosition) // avoid going back
				{
					return;
				}

				DiscoveredCell newDiscovery = Discover(localPos);
				newDiscoveries.push_back(std::move(newDiscovery));
			});
		}

		u32 DetectLoop(Vector<DiscoveredCell>& discoveredCells) const
		{
			auto it = std::find(discoveredCells.begin(), discoveredCells.end(), *this);
			if (it != discoveredCells.end())
			{
				const DiscoveredCell& otherLoopEnd = *it;
				const u32 loopStepCount = otherLoopEnd.StepsTaken + StepsTaken;
				return loopStepCount;
			}
			return 0;
		}

		static u32 TryDiscoverLoop(const DiscoveredCell& start)
		{
			Vector<DiscoveredCell> log;
			Vector<DiscoveredCell> currentHeads;
			Vector<DiscoveredCell> nextHeads;

			currentHeads.push_back(start);

			while (true)
			{
				// discover
				for (const DiscoveredCell& current : currentHeads)
				{
					current.DiscoverNeighbors(nextHeads);
				}

				log.insert(log.end(), currentHeads.begin(), currentHeads.end());

				// check loop
				for (const DiscoveredCell& newDiscovery : nextHeads)
				{
					const u32 loopStepCount = newDiscovery.DetectLoop(log);
					if (loopStepCount > 0)
					{
						// success
						return loopStepCount;
					}
				}

				if (nextHeads.empty())
				{
					// dead ends, no loop discovered
					return 0;
				}

				currentHeads = nextHeads;
				nextHeads.clear();
			}
		}

		u32 DiscoverLoopFromHere()
		{
			return TryDiscoverLoop(*this);
		}
	};

	export String ExecutePart1()
	{
		auto input = OpenInput("day10.txt");

		DiscoveredCell::GlobalMap map;
		DiscoveredCell::GlobalMap::Base& mapUnderlyingData = map.UnderlyingData();

		Vector<vec2> startPositions;

		u32 verticalIndex = 0;
		String line;
		while (std::getline(input, line))
		{
			// underlying array is inverted to make this cheaper, probably complete nonesense but too late
			auto& mapUnderlyingDataRow = mapUnderlyingData[verticalIndex];
			for (u32 horizontalIndex = 0; horizontalIndex < DiscoveredCell::s_GlobalHorizontalSize; horizontalIndex++)
			{
				const char cellContentChar = line[horizontalIndex];
				CellContent cellContent = CellContent::Invalid;

				switch (cellContentChar)
				{
					case '|': cellContent = CellContent::PipeVertical; break;
					case '-': cellContent = CellContent::PipeHorizontal; break;
					case 'L': cellContent = CellContent::PipeNorthEastL; break;
					case 'J': cellContent = CellContent::PipeNorthWestJ; break;
					case '7': cellContent = CellContent::PipeSouthWest7; break;
					case 'F': cellContent = CellContent::PipeSouthEastF; break;
					case 'S': cellContent = CellContent::PipeStart; break;
					case '.': cellContent = CellContent::Ground; break;
				}
				Assert(cellContent != CellContent::Invalid);

				mapUnderlyingDataRow[horizontalIndex] = cellContent;

				if (cellContent == CellContent::PipeStart)
				{
					startPositions.emplace_back(horizontalIndex, verticalIndex);
				}
			}

			verticalIndex++;
		}
		Assert(verticalIndex == DiscoveredCell::s_GlobalVerticalSize);

		Assert(startPositions.size() == 1);

		DiscoveredCell navigator(map, startPositions[0]);
		u32 startToStartLoopStepCount = navigator.DiscoverLoopFromHere();
		Assert(startToStartLoopStepCount % 2 == 0);
		u32 loopFurthestDistanceSteps = startToStartLoopStepCount / 2;

		return std::to_string(loopFurthestDistanceSteps);
	}

	export String ExecutePart2()
	{
		return "";
	}
}