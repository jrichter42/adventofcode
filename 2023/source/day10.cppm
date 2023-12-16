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

	Vec2I DirectionToVec2(Direction dir)
	{
		switch (dir)
		{
		case Direction::North: return Vec2I(0, -1);
		case Direction::East: return Vec2I(1, 0);
		case Direction::West: return Vec2I(-1, 0);
		case Direction::South: return Vec2I(0, 1);
		}
		return Vec2I();
	}

	Direction Vec2ToDirection(const Vec2I& vec)
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

		CellContent operator[](const Vec2I& position) const
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

		DiscoveredCell(const GlobalMap& globalMap, Vec2I globalPos)
			: GlobalMapRef(const_cast<GlobalMap&>(globalMap))
			, Position(globalPos)
		{
			Content = Peek(Vec2I());
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

		Vec2I Position;
		CellContent Content = CellContent::Invalid;
		Vec2I DiscoveredFromLocalPosition;
		u32 StepsTaken = 0;
	protected:
		GlobalMap& GlobalMapRef; // should be const but c++
	public:

		CellContent Peek(const Vec2I& localPos, bool localOnly = false) const
		{
			const Vec2I localLimitBottom = Vec2I(
				Position.X == 0 ? 0 : Position.X - 1,
				Position.Y == 0 ? 0 : Position.Y - 1);

			const Vec2I localLimitTop = Vec2I(
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

			const Vec2I globalPos = Position + localPos;

			return GlobalMapRef[globalPos];
		}

		DiscoveredCell Discover(const Vec2I& localPos) const
		{
			const Vec2I globalPos = Position + localPos;

			DiscoveredCell result(GlobalMapRef, globalPos);
			result.DiscoveredFromLocalPosition = -localPos;
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

				const Vec2I localPos = DirectionToVec2(direction);
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
			Vector<Vec2I> connectedNeighbors;
			ForEachConnectedNeighbor([&](const Direction direction, const Vec2I& localPos, const CellContent cellContent) {
				if (cellContent == CellContent::Invalid
					|| localPos == DiscoveredFromLocalPosition) // avoid going back
				{
					return;
				}

				DiscoveredCell newDiscovery = Discover(localPos);
				newDiscoveries.push_back(std::move(newDiscovery));
			});
		}

		u32 DetectLoop(Vector<DiscoveredCell>& discoveredCells, Vector<Vec2I>& outLoopPath) const
		{
			auto it = std::find(discoveredCells.begin(), discoveredCells.end(), *this);
			if (it != discoveredCells.end())
			{
				const DiscoveredCell& otherLoopEnd = *it;
				const u32 loopStepCount = otherLoopEnd.StepsTaken + StepsTaken;

				// part 2 hack: backtrack from newDiscovery
				{
					auto stepBack = [&](const DiscoveredCell& cell) -> const DiscoveredCell* {
						const Vec2I& previousPos = cell.Position + cell.DiscoveredFromLocalPosition;

						if (cell.DiscoveredFromLocalPosition == Vec2I())
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

					Vector<Vec2I> rollBackToStart;

					auto rollBackFromAndWrite = [&](const DiscoveredCell& from, Vector<Vec2I>& outPath) {
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

		static u32 TryDiscoverLoop(const DiscoveredCell& start, Vector<Vec2I>& outLoopPath)
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

		u32 DiscoverLoopFromHere(Vector<Vec2I>& outLoopPath)
		{
			return TryDiscoverLoop(*this, outLoopPath);
		}
	};

	export String ExecutePart1()
	{
		auto input = OpenInput("day10.txt");

		DiscoveredCell::GlobalMap map;
		DiscoveredCell::GlobalMap::Base& mapUnderlyingData = map.UnderlyingData();

		Vector<Vec2I> startPositions;

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
		Vector<Vec2I> loopPath; // I'm too lazy for nullptr checks. Greetings from part 2.
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

		Vector<Vec2I> startPositions;

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

		Vector<Vec2I> loopPath;
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
		for (const Vec2I& pos : loopPath)
		{
			contentTypeMap[pos.X][pos.Y] = ContentType::Loop;
		}

		Vector<Vec2I> writtenInsidePositions;
		constexpr Rotation insideDirRot = Rotation::Right; // two options, no I will not calculate this.
		for (u32 loopElemIdx = 0; loopElemIdx < loopPath.size() - 1; loopElemIdx++)
		{
			const Vec2I& pos = loopPath[loopElemIdx];
			CellContent content = map[pos];
			Vector<Direction> contentConnections = ContentConnections(content);

			const Vec2I& nextPos = loopPath[loopElemIdx + 1];
			CellContent nextContent = map[nextPos];
			Vector<Direction> nextContentConnections = ContentConnections(nextContent);

			Vec2I directionVector = nextPos - pos;;

			Direction loopDir = Vec2ToDirection(directionVector);
			Assert(loopDir != Direction::Invalid);

			Direction insideDir = Rotate(loopDir, insideDirRot);
			Assert(insideDir != Direction::Invalid);
			Vec2I insideDirVec = DirectionToVec2(insideDir);

			auto tryWrite = [&](const Vec2I& pos) {
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
						Vec2I loopDirVec = DirectionToVec2(loopDir);
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

		auto tryFill = [&](u32 x, u32 y, Vector<Vec2I>& log) -> bool {
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

			const Vec2I pos = Vec2I(x, y);
			log.push_back(pos);
			writtenInsidePositions.push_back(pos);
			return true;
		};

		Vector<Vec2I> floodFillCurrentHeads = writtenInsidePositions;
		Vector<Vec2I> floodFillNextHeads;
		while (floodFillCurrentHeads.empty() == false)
		{
			Assert(floodFillCurrentHeads.size() < (DiscoveredCell::s_GlobalHorizontalSize* DiscoveredCell::s_GlobalVerticalSize));

			for (const Vec2I& currentHead : floodFillCurrentHeads)
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