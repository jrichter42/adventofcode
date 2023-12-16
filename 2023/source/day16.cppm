export module day16;

import common;

export namespace aoc
{
	enum class TileContent
	{
		Invalid,
		Empty,
		MirrorForwardSlash,
		MirrorBackslash,
		SplitterVertical,
		SplitterHorizontal
	};

	TileContent CharToTileContent(char c)
	{
		switch (c)
		{
			case '.': return TileContent::Empty;
			case '/': return TileContent::MirrorForwardSlash;
			case '\\': return TileContent::MirrorBackslash;
			case '|': return TileContent::SplitterVertical;
			case '-': return TileContent::SplitterHorizontal;
			default: Assert(false);
		}
		return TileContent::Invalid;
	}

	enum class Direction
	{
		Invalid,
		Right,
		Down,
		Left,
		Up
	};

	Vec2I DirToVec2(Direction dir)
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

	char DirToDebugChar(Direction dir)
	{
		switch (dir)
		{
			case Direction::Right: return '>';
			case Direction::Down: return 'v';
			case Direction::Left: return '<';
			case Direction::Up: return '^';
		}
		return '+';
	}

	Vec2I GetPosInDir(const Vec2I& posFrom, Direction dir)
	{
		switch (dir)
		{
			case Direction::Right: return Vec2I(1, 0);
			case Direction::Down: return Vec2I(0, 1);
			case Direction::Left: return Vec2I(-1, 0);
			case Direction::Up: return Vec2I(0, -1);
		}
		return {};
	}

	struct Tile
	{
		Tile(TileContent content)
			: Content(content)
		{}

		TileContent Content = TileContent::Invalid;
		Vector<Direction> IncomingBeamDirections;

		bool IsEnergized() const
		{
			return IncomingBeamDirections.empty() == false;
		}

		// returns outgoing dir(s)
		Vector<Direction> AcceptBeam(Direction incomingDir)
		{
			IncomingBeamDirections.push_back(incomingDir);

			const Vector<Direction> outgoingDirs = [&]() -> Vector<Direction> {
				switch (Content)
				{
					case TileContent::Empty:
					{
						return { incomingDir };
					}; break;
					case TileContent::MirrorForwardSlash:
					{
						switch (incomingDir)
						{
							case Direction::Right: return { Direction::Up };
							case Direction::Down: return { Direction::Left };
							case Direction::Left: return { Direction::Down };
							case Direction::Up: return { Direction::Right };
						}
					}; break;
					case TileContent::MirrorBackslash:
					{
						switch (incomingDir)
						{
							case Direction::Right: return { Direction::Down };
							case Direction::Down: return { Direction::Right };
							case Direction::Left: return { Direction::Up };
							case Direction::Up: return { Direction::Left };
						}
					}; break;
					case TileContent::SplitterVertical:
					{
						switch (incomingDir)
						{
							case Direction::Up:
							case Direction::Down:
							{
								return { incomingDir };
							}
							case Direction::Left:
							case Direction::Right:
							{
								return { Direction::Up, Direction::Down };
							}
						}
					}; break;
					case TileContent::SplitterHorizontal:
					{
						switch (incomingDir)
						{
							case Direction::Left:
							case Direction::Right:
							{
								return { incomingDir };
							}
							case Direction::Up:
							case Direction::Down:
							{
								return { Direction::Left, Direction::Right };
							}
						}
					}; break;
					default:
					{
						Assert(false);
					} break;
				}
				return {};
			}();

			//IncomingBeamDirections.insert(BeamDirections.end(), outgoingDirs.begin(), outgoingDirs.end());

			return outgoingDirs;
		}
	};

	struct Map
	{
		// row major
		Vector<Vector<Tile>> Tiles;

		u32 Width() const
		{
			return NarrowSizeT(Tiles.size());
		}

		u32 Height() const
		{
			if (Tiles.empty())
			{
				return 0;
			}

			return NarrowSizeT(Tiles[0].size());
		}

		Vec2I Size() const
		{
			return Vec2I(Width(), Height());
		}

		void PropagateBeam(const Vec2I& pos = Vec2I(0, 0), const Direction incomingDir = Direction::Right)
		{
			Vector<std::pair<Vec2I, Direction>> currentBeamHeads = { std::make_pair(pos, incomingDir) };

			Vector<std::pair<Vec2I, Direction>> nextBeamHeads;

			const Vec2I mapSize = Size();
			while (currentBeamHeads.empty() == false)
			{
				for (const auto& currentBeamHead : currentBeamHeads)
				{
					const Vec2I& currentPos = currentBeamHead.first;
					const Direction currentDir = currentBeamHead.second;

					if (currentPos.X < 0
						|| currentPos.Y < 0
						|| currentPos.X >= mapSize.X
						|| currentPos.Y >= mapSize.Y)
					{
						// End of this beam (outside map)
						if constexpr (IsDebug()) { LogFormat("{} {},{} X", DirToDebugChar(currentDir), currentPos.X + 1, currentPos.Y + 1); }
						continue;
					}

					Tile& tile = Tiles[currentPos.Y][currentPos.X];

					auto tileBeamDirIt = std::find(tile.IncomingBeamDirections.begin(), tile.IncomingBeamDirections.end(), currentDir);
					if (tileBeamDirIt != tile.IncomingBeamDirections.end())
					{
						// Loop found
						if constexpr (IsDebug()) { LogFormat("{} {},{} <>", DirToDebugChar(currentDir), currentPos.X + 1, currentPos.Y + 1); }
						continue;
					}

					Vector<Direction> nextDirections = tile.AcceptBeam(currentDir);
					Assert(nextDirections.empty() == false);

					if constexpr (IsDebug()) { LogFormat("{} {},{} {}", DirToDebugChar(currentDir), currentPos.X + 1, currentPos.Y + 1, nextDirections.size()); }

					for (Direction dir : nextDirections)
					{
						nextBeamHeads.emplace_back(currentPos + DirToVec2(dir), dir);
					}
				}

				currentBeamHeads = nextBeamHeads;
				nextBeamHeads.clear();
			}
		}

		u32 GetEnergizedCount() const
		{
			u32 result = 0;
			for (const auto& row : Tiles)
			{
				for (const auto& tile : row)
				{
					if (tile.IsEnergized())
					{
						++result;
					}
				}
			}
			return result;
		}
	};

	export String ExecutePart1()
	{
		auto input = OpenInput("day16.txt");

		Map map;

		String line;
		while (std::getline(input, line))
		{
			auto& row = map.Tiles.emplace_back();
			for (const char c : line)
			{
				const TileContent content = CharToTileContent(c);
				row.push_back(content);
			}
		}

		map.PropagateBeam();

		u32 result = map.GetEnergizedCount();

		return std::to_string(result);
	}

	export String ExecutePart2()
	{
		auto input = OpenInput("day16.txt");

		Map map;

		String line;
		while (std::getline(input, line))
		{
			auto& row = map.Tiles.emplace_back();
			for (const char c : line)
			{
				const TileContent content = CharToTileContent(c);
				row.push_back(content);
			}
		}

		u32 result = 0;

		auto execute = [&](u32 x, u32 y, Direction dir) {
			Map mapCopy = map;
			mapCopy.PropagateBeam(Vec2I(x, y), dir);
			result = std::max(result, mapCopy.GetEnergizedCount());
		};

		{
			u32 y = 0;
			for (u32 x = 0; x < map.Width(); ++x) { execute(x, y, Direction::Down); }

			y = map.Height() - 1;
			for (u32 x = 0; x < map.Width(); ++x) { execute(x, y, Direction::Up); }
		}

		{
			u32 x = 0;
			for (u32 y = 0; y < map.Height(); ++y) { execute(x, y, Direction::Right); }

			x = map.Width() - 1;
			for (u32 y = 0; y < map.Height(); ++y) { execute(x, y, Direction::Left); }
		}

		return std::to_string(result);
	}
}