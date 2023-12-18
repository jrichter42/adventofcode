export module day18;

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
		if (dir == Direction::Up && rot == Rotation::Left)
		{
			return Direction::Left;
		}
		else if (dir == Direction::Left && rot == Rotation::Right)
		{
			return Direction::Up;
		}

		using DirUnderlyingT = std::underlying_type_t<Direction>;
		DirUnderlyingT result = static_cast<DirUnderlyingT>(dir) + rotInt;
		return static_cast<Direction>(result);
	}

	struct GroundVolume
	{
		Vec2I Position;
		bool DugOut = false;
		Direction DugOutDir = Direction::Invalid;
		String Color;
	};

	struct Ground
	{
		Ground()
		{
			ExpandVolumes(Vec2I(1, 1));
		}

		Vec2I DiggerPos;

		Vector<Vector<GroundVolume>> Volumes;

		u32 Height() const
		{
			return NarrowSizeT(Volumes.size());
		}

		u32 Width() const
		{
			if (Volumes.empty())
			{
				return 0;
			}

			return NarrowSizeT(Volumes[0].size());
		}

		Vec2I Size() const
		{
			return Vec2I(Width(), Height());
		}

		GroundVolume& Access(Vec2I pos)
		{
			return Volumes[pos.Y][pos.X];
		}

		void ShiftVolumes(Vec2I offset)
		{
			// assuming GroundVolumes dont have their position set yet

			Assert(offset.X >= 0 && offset.Y >= 0);

			if (offset.X > 0)
			{
				for (auto& row : Volumes)
				{
					row.insert(row.begin(), offset.X, {});
				}
			}

			Vector<GroundVolume> newRow(Width() + offset.X);
			if (offset.Y > 0)
			{
				Volumes.insert(Volumes.begin(), offset.Y, newRow);
			}

			DiggerPos += offset;
		}

		void ExpandVolumes(Vec2I offset)
		{
			Assert(offset.X >= 0 && offset.Y >= 0);

			if (offset.X > 0)
			{
				for (auto& row : Volumes)
				{
					row.insert(row.end(), offset.X, {});
				}
			}

			Vector<GroundVolume> newRow(Width() + offset.X);
			if (offset.Y > 0)
			{
				Volumes.insert(Volumes.end(), offset.Y, newRow);
			}
		}

		void Dig(Direction dir, u32 meters, String color)
		{
			if (dir == Direction::Left
				|| dir == Direction::Up)
			{
				Vec2I digTargetReachOutsideMap = DiggerPos + ( DirToVec2(dir) * meters );
				if (digTargetReachOutsideMap.X > 0)
				{
					digTargetReachOutsideMap.X = 0;
				}
				if (digTargetReachOutsideMap.Y > 0)
				{
					digTargetReachOutsideMap.Y = 0;
				}

				if (Length(digTargetReachOutsideMap) > 0)
				{
					ShiftVolumes(-digTargetReachOutsideMap);
				}
			}
			else
			{
				Vec2I digTargetReachOutsideMap = DiggerPos + ( DirToVec2(dir) * meters ) - Size() + Vec2I(1, 1);
				if (digTargetReachOutsideMap.X < 0)
				{
					digTargetReachOutsideMap.X = 0;
				}
				if (digTargetReachOutsideMap.Y < 0)
				{
					digTargetReachOutsideMap.Y = 0;
				}

				if (Length(digTargetReachOutsideMap) > 0)
				{
					ExpandVolumes(digTargetReachOutsideMap);
				}
			}

			for (u32 i = 0; i < meters; ++i)
			{
				DiggerPos += DirToVec2(dir);

				GroundVolume& volume = Access(DiggerPos);
				volume.DugOut = true;
				volume.DugOutDir = dir;
				volume.Color = color;
			}
		}

		u32 DigInsideArea(const Vector<GroundVolume*>& dugOutVolumes)
		{
			// copied from day 10

			u32 result = 0;

			auto tryWrite = [&](const Vec2I& pos, Vector<Vec2I>& log) {
				if (pos.X < 0
					|| pos.Y < 0
					|| ToUnsigned(pos.X) >= u32(Width())
					|| ToUnsigned(pos.Y) >= u32(Height()))
				{
					return false;
				}

				GroundVolume& volume = Access(pos);
				if (volume.DugOut)
				{
					return false;
				}

				volume.DugOut = true;
				log.push_back(pos);
				++result;
				return true;
			};

			Vector<Vec2I> writtenInsidePositions;

			constexpr Rotation insideDirRot = Rotation::Right; // two options, no I will not calculate this.
			for (const GroundVolume* dugOutVolumePtr : dugOutVolumes)
			{
				Assert(dugOutVolumePtr != nullptr);
				const GroundVolume& dugOutVolume = *dugOutVolumePtr;
				Assert(dugOutVolume.DugOut);

				const Vec2I& pos = dugOutVolume.Position;

				Direction dugOutDir = dugOutVolume.DugOutDir;
				Assert(dugOutDir != Direction::Invalid);

				Direction insideDir = Rotate(dugOutDir, insideDirRot);
				Assert(insideDir != Direction::Invalid);

				const Vec2I insideDirVec = DirToVec2(insideDir);
				tryWrite(pos + insideDirVec, writtenInsidePositions);
			}

			// flood fill from all InsideLoop until Loop or InsideLoop

			Vector<Vec2I> floodFillCurrentHeads = writtenInsidePositions;
			Vector<Vec2I> floodFillNextHeads;
			while (floodFillCurrentHeads.empty() == false)
			{
				Assert(floodFillCurrentHeads.size() < (Width() * Height()));

				for (const Vec2I& currentHead : floodFillCurrentHeads)
				{
					tryWrite(currentHead + DirToVec2(Direction::Left), floodFillNextHeads);
					tryWrite(currentHead + DirToVec2(Direction::Up), floodFillNextHeads);
					tryWrite(currentHead + DirToVec2(Direction::Right), floodFillNextHeads);
					tryWrite(currentHead + DirToVec2(Direction::Down), floodFillNextHeads);
				}

				floodFillCurrentHeads = floodFillNextHeads;
				floodFillNextHeads.clear();
			}

			return result;
		}

		u32 FinalizeDigging()
		{
			Vector<GroundVolume*> dugOutVolumes;
			for (u32 y = 0; y < Height(); ++y)
			{
				auto& row = Volumes[y];
				String rowStr;
				for (u32 x = 0; x < Width(); ++x)
				{
					GroundVolume& volume = row[x];
					volume.Position = Vec2I(x, y);
					if (volume.DugOut)
					{
						dugOutVolumes.push_back(&volume);
					}
				}
			}

			DebugVisualize();
			Log("");

			const u32 dugOutInsideCount = DigInsideArea(dugOutVolumes);

			DebugVisualize();
			Log("");

			return NarrowSizeT(dugOutVolumes.size()) + dugOutInsideCount;
		}

		void DebugVisualize()
		{
			if constexpr (IsDebug() == false)
			{
				return;
			}

			for (const auto& row : Volumes)
			{
				String rowStr;
				for (const auto& volume : row)
				{
					if (volume.DugOut)
					{
						rowStr.push_back('#');
					}
					else
					{
						rowStr.push_back('.');
					}
				}
				Log(rowStr);
			}
		}
	};

	export String ExecutePart1()
	{
		auto input = OpenInput("day18.txt");

		Ground ground;

		String line;
		while (std::getline(input, line))
		{
			Vector<String> digInstructionStrs = Split(line, " ");
			Assert(digInstructionStrs.size() == 3);
			const String& digDirStr = digInstructionStrs[0];
			const String& digMetersStr = digInstructionStrs[1];
			const String& digColorStr = digInstructionStrs[2];

			Assert(digDirStr.size() == 1);
			Direction digDir = Direction::Invalid;
			switch (digDirStr[0])
			{
				case 'U': digDir = Direction::Up; break;
				case 'D': digDir = Direction::Down; break;
				case 'L': digDir = Direction::Left; break;
				case 'R': digDir = Direction::Right; break;
			}
			Assert(digDir != Direction::Invalid);

			u32 digMeters = std::stoi(digMetersStr);
			Assert(digMeters > 0);

			String digColor = Split(Split(Split(digColorStr, ")")[0], "(")[0], "#")[0]; // yes, this is what I did. deal with it.

			ground.Dig(digDir, digMeters, digColor);
		}

		u32 result = ground.FinalizeDigging();

		return std::to_string(result);
	}

	export String ExecutePart2()
	{
		return "";
	}
}