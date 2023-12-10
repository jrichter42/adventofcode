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

	enum class ContentType
	{
		Invalid,
		Loop,
		InsideLoop,
		OutsideLoop
	};

	enum class Direction
	{
		Invalid,
		North,
		East,
		South,
		West
	};

	Vector<Direction> ContentConnections(CellContent ct)
	{
		switch (ct)
		{
			case CellContent::PipeVertical: return { Direction::East, Direction::West };
			case CellContent::PipeHorizontal: return { Direction::North, Direction::South };
			case CellContent::PipeNorthEastL: return { Direction::North, Direction::East };
			case CellContent::PipeNorthWestJ: return { Direction::North, Direction::West };
			case CellContent::PipeSouthWest7: return { Direction::South, Direction::West };
			case CellContent::PipeSouthEastF: return { Direction::South, Direction::East };
			case CellContent::PipeStart: return { Direction::North, Direction::East, Direction::West, Direction::South };
		}
		return {};
	}

	enum class Rotation
	{
		Invalid,
		Left,
		Right
	};

	Direction Rotate(Direction dir, Rotation rot)
	{
		s32 rotInt = 0;
		if (rot == Rotation::Left)
		{
			rotInt = -1;
		}
		else if (rot == Rotation::Right)
		{
			rotInt = 1;
		}

		// too lazy to deal with wrap around in combination with safe usage of underlying and its value range
		if (dir == Direction::North && rot == Rotation::Left)
		{
			return Direction::West;
		}
		else if (dir == Direction::West && rot == Rotation::Right)
		{
			return Direction::North;
		}

		using DirUnderlyingT = std::underlying_type_t<Direction>;
		DirUnderlyingT result = static_cast<DirUnderlyingT>(dir) + rotInt;
		return static_cast<Direction>(result);
	}

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

		vec2 operator+(const vec2& other) const
		{
			return vec2(
				X + other.X,
				Y + other.Y);
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

	Direction Vec2ToDirection(const vec2& vec)
	{
		if (vec.X == 0)
		{
			if (vec.Y == -1)
			{
				return Direction::North;
			}
			else if (vec.Y == 1)
			{
				return Direction::South;
			}
		}
		else if (vec.Y == 0)
		{
			if (vec.X == 1)
			{
				return Direction::East;
			}
			else if (vec.X == -1)
			{
				return Direction::West;
			}
		}
		return Direction::Invalid;
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
		using ContentTypeMap = Array<Array<ContentType, DiscoveredCell::s_GlobalVerticalSize>, DiscoveredCell::s_GlobalHorizontalSize>;

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

		u32 DetectLoop(Vector<DiscoveredCell>& discoveredCells, Vector<vec2>& outLoopPath) const
		{
			auto it = std::find(discoveredCells.begin(), discoveredCells.end(), *this);
			if (it != discoveredCells.end())
			{
				const DiscoveredCell& otherLoopEnd = *it;
				const u32 loopStepCount = otherLoopEnd.StepsTaken + StepsTaken;

				// part 2 hack: backtrack from newDiscovery
				{
					auto stepBack = [&](const DiscoveredCell& cell) -> const DiscoveredCell* {
						const vec2& previousPos = vec2( // I should finally implement stupid vec2 arithmetic
							cell.Position.X + cell.DiscoveredFromLocalPosition.X,
							cell.Position.Y + cell.DiscoveredFromLocalPosition.Y);

						if (cell.DiscoveredFromLocalPosition == vec2())
						{
							// start reached
							return nullptr;
						}

						auto it = std::find_if(discoveredCells.begin(), discoveredCells.end(), [&](const DiscoveredCell& cell) {
							return cell.Position == previousPos;
						});

						if (it != discoveredCells.end())
						{
							const DiscoveredCell& ref = *it;
							return &ref;
						}

						return nullptr;
					};

					Vector<vec2> rollBackToStart;

					auto rollBackFromAndWrite = [&](const DiscoveredCell& from, Vector<vec2>& outPath) {
						const DiscoveredCell* previousStep = stepBack(from);
						while (previousStep != nullptr)
						{
							outPath.push_back(previousStep->Position);
							previousStep = stepBack(*previousStep);
						}
					};

					rollBackFromAndWrite(*this, rollBackToStart); // current intersection (exclusive) -> taken path -> start (inclusive)

					// start (inclusive) -> taken path -> current intersection (exclusive)
					for (auto rIt = rollBackToStart.rbegin(); rIt != rollBackToStart.rend(); ++rIt)
					{
						outLoopPath.push_back(*rIt);
					}

					// intersection
					outLoopPath.push_back(Position);

					// current instersection (exclusive) -> found path -> start (inclusive)
					rollBackFromAndWrite(otherLoopEnd, outLoopPath);
				}

				return loopStepCount;
			}
			return 0;
		}

		static u32 TryDiscoverLoop(const DiscoveredCell& start, Vector<vec2>& outLoopPath)
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
					const u32 loopStepCount = newDiscovery.DetectLoop(log, outLoopPath);
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

		u32 DiscoverLoopFromHere(Vector<vec2>& outLoopPath)
		{
			return TryDiscoverLoop(*this, outLoopPath);
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
		Vector<vec2> loopPath; // I'm too lazy for nullptr checks. Greetings from part 2.
		u32 startToStartLoopStepCount = navigator.DiscoverLoopFromHere(loopPath);
		Assert(startToStartLoopStepCount % 2 == 0);
		u32 loopFurthestDistanceSteps = startToStartLoopStepCount / 2;

		return std::to_string(loopFurthestDistanceSteps);
	}

	export String ExecutePart2()
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

		Vector<vec2> loopPath;
		DiscoveredCell navigator(map, startPositions[0]);
		u32 startToStartLoopStepCount = navigator.DiscoverLoopFromHere(loopPath);
		Assert(startToStartLoopStepCount > 0);

		Assert(loopPath.front() == loopPath.back()); // start == start

		DiscoveredCell::ContentTypeMap contentTypeMap;
		for (auto& array : contentTypeMap)
		{
			std::fill(array.begin(), array.end(), ContentType::OutsideLoop);
		}

		// write loop so we can observe it when writing insides
		for (const vec2& pos : loopPath)
		{
			contentTypeMap[pos.X][pos.Y] = ContentType::Loop;
		}

		Vector<vec2> writtenInsidePositions;
		constexpr Rotation insideDirRot = Rotation::Right; // two options, no I will not calculate this.
		for (u32 loopElemIdx = 0; loopElemIdx < loopPath.size() - 1; loopElemIdx++)
		{
			const vec2& pos = loopPath[loopElemIdx];
			CellContent content = map[pos];
			Vector<Direction> contentConnections = ContentConnections(content);

			const vec2& nextPos = loopPath[loopElemIdx + 1];
			CellContent nextContent = map[nextPos];
			Vector<Direction> nextContentConnections = ContentConnections(nextContent);

			vec2 directionVector(nextPos.X - pos.X, nextPos.Y - pos.Y);

			Direction loopDir = Vec2ToDirection(directionVector);
			Assert(loopDir != Direction::Invalid);

			Direction insideDir = Rotate(loopDir, insideDirRot);
			Assert(insideDir != Direction::Invalid);
			vec2 insideDirVec = DirectionToVec2(insideDir);

			auto tryWrite = [&](const vec2& pos) {
				ContentType& type = contentTypeMap[pos.X][pos.Y];
				if (type == ContentType::Loop
					|| type == ContentType::InsideLoop)
				{
					return false;
				}

				type = ContentType::InsideLoop;
				writtenInsidePositions.push_back(pos);
				return true;
			};

			{
			// example: from pipe to top left, insideDirRot == Left
			// . . .
			// . F . <- to
			// . | . <- from

				// from
				// . . .
				// . F .
				// X | .
				if (Contains(contentConnections, insideDir) == false)
				{
					tryWrite(pos + insideDirVec);
				}

				// to1
				// . . .
				// X F .
				// . | .
				if (Contains(nextContentConnections, insideDir) == false)
				{
					tryWrite(nextPos + insideDirVec);

					// to2
					// X . .
					// . F .
					// . | .
					if (Contains(nextContentConnections, loopDir) == false)
					{
						vec2 loopDirVec = DirectionToVec2(loopDir);
						tryWrite(nextPos + insideDirVec + loopDirVec);
					}
				}

				// handled by next loops from:
				// . X .
				// . F .
				// . | .
			}

		}

		// flood fill from all InsideLoop until Loop or InsideLoop

		auto tryFill = [&](u32 x, u32 y, Vector<vec2>& log) -> bool {
			if (x > DiscoveredCell::s_GlobalHorizontalLimit
				|| y > DiscoveredCell::s_GlobalVerticalLimit)
			{
				return false;
			}

			ContentType& type = contentTypeMap[x][y];
			if (type == ContentType::Loop
				|| type == ContentType::InsideLoop)
			{
				return false;
			}

			type = ContentType::InsideLoop;

			const vec2 pos = vec2(x, y);
			log.push_back(pos);
			writtenInsidePositions.push_back(pos);
			return true;
		};

		Vector<vec2> floodFillCurrentHeads = writtenInsidePositions;
		Vector<vec2> floodFillNextHeads;
		while (floodFillCurrentHeads.empty() == false)
		{
			Assert(floodFillCurrentHeads.size() < (DiscoveredCell::s_GlobalHorizontalSize* DiscoveredCell::s_GlobalVerticalSize));

			for (const vec2& currentHead : floodFillCurrentHeads)
			{
				if (currentHead.X > 0)
				{
					// west
					tryFill(currentHead.X - 1, currentHead.Y, floodFillNextHeads);
				}

				if (currentHead.Y > 0)
				{
					// north
					tryFill(currentHead.X, currentHead.Y - 1, floodFillNextHeads);
				}

				// east
				tryFill(currentHead.X + 1, currentHead.Y, floodFillNextHeads);
				// south
				tryFill(currentHead.X, currentHead.Y + 1, floodFillNextHeads);
			}

			floodFillCurrentHeads = floodFillNextHeads;
			floodFillNextHeads.clear();
		}

		if constexpr (IsDebug())
		{
			for (auto& array : contentTypeMap)
			{
				String row;
				for (auto cell : array)
				{
					Assert(cell != ContentType::Invalid);
					switch (cell)
					{
						case ContentType::Loop: row.append("+"); break;
						case ContentType::InsideLoop: row.append("#"); break;
						case ContentType::OutsideLoop: row.append("."); break;
					}
				}
				Log(row);
			}
		}

		return std::to_string(writtenInsidePositions.size());
	}
}