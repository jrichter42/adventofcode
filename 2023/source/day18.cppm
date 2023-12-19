module;

#include <stdio.h>

export module day18;

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

	enum class Rotation
	{
		Invalid,
		Left,
		Right
	};

	constexpr Direction Rotate(Direction dir, Rotation rot)
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

	struct Ground
	{
		using GroundVolume = std::pair<Vec2I, Direction>;

		Vec2I DiggerPos;
		Vector<GroundVolume> DugOutVolumes;

		struct
		{
			bool operator()(const GroundVolume& lhs, const Vec2I& rhs) const
			{
				return lhs.first < rhs;
			}

			bool operator()(const Vec2I& lhs, const GroundVolume& rhs) const
			{
				return lhs < rhs.first;
			}
		} CompPosLess;

		void Dig(Direction dir, u64 meters)
		{
			if constexpr (IsDebug())
			{
				LogFormat("Digging {} meters", meters);
			}

			for (u64 i = 0; i < meters; ++i)
			{
				DiggerPos += DirToVec2(dir);

				DugOutVolumes.push_back(std::make_pair(DiggerPos, dir));
			}

			std::sort(DugOutVolumes.begin(), DugOutVolumes.end());
			DugOutVolumes.erase(std::unique(DugOutVolumes.begin(), DugOutVolumes.end()), DugOutVolumes.end());
		}

		u64 DigInsideArea()
		{
			// impl very similar to day 10

			Vector<Vec2I> dugOutPositions;
			for (GroundVolume& volume : DugOutVolumes)
			{
				dugOutPositions.push_back(volume.first);
			}

			std::ranges::sort(dugOutPositions);

			if constexpr (IsDebug())
			{
				LogFormat("Dug out {}. Starting digging inside.", dugOutPositions.size());
			}

			auto tryDig = [&](const Vec2I& pos, Vector<Vec2I>& log) {
				if (std::ranges::binary_search(dugOutPositions, pos) == false)
				{
					log.push_back(pos);
					dugOutPositions.insert(std::ranges::upper_bound(dugOutPositions, pos), pos);
				}
			};

			Vector<Vec2I> writtenInsidePositions;

			for (const GroundVolume& volume : DugOutVolumes)
			{
				// Rotation: two options, no I will not calculate this.
				const Vec2I pos = volume.first + DirToVec2(Rotate(volume.second, Rotation::Right));
				if (std::ranges::binary_search(dugOutPositions, pos) == false)
				{
					writtenInsidePositions.push_back(pos);
					dugOutPositions.insert(std::ranges::upper_bound(dugOutPositions, pos), pos);
				}
			}

			if constexpr (IsDebug())
			{
				LogFormat("Dug out {}. Starting flood digging.", dugOutPositions.size());
			}

			// flood fill/dig from all InsideLoop until Loop or InsideLoop

			Vector<Vec2I> floodFillCurrentHeads = writtenInsidePositions;
			Vector<Vec2I> floodFillNextHeads;
			Vec2I pos;
			while (floodFillCurrentHeads.empty() == false)
			{
				if constexpr (IsDebug())
				{
				//LogFormat("Flood dig heads: {} Dug out: {}", floodFillCurrentHeads.size(), dugOutPositions.size());
				}

				for (const Vec2I& currentHead : floodFillCurrentHeads)
				{
					pos = currentHead + DirToVec2(Direction::Left);
					if (std::ranges::binary_search(dugOutPositions, pos) == false)
					{
						floodFillNextHeads.push_back(pos);
						dugOutPositions.insert(std::ranges::upper_bound(dugOutPositions, pos), pos);
					}

					pos = currentHead + DirToVec2(Direction::Up);
					if (std::ranges::binary_search(dugOutPositions, pos) == false)
					{
						floodFillNextHeads.push_back(pos);
						dugOutPositions.insert(std::ranges::upper_bound(dugOutPositions, pos), pos);
					}

					pos = currentHead + DirToVec2(Direction::Right);
					if (std::ranges::binary_search(dugOutPositions, pos) == false)
					{
						floodFillNextHeads.push_back(pos);
						dugOutPositions.insert(std::ranges::upper_bound(dugOutPositions, pos), pos);
					}

					pos = currentHead + DirToVec2(Direction::Down);
					if (std::ranges::binary_search(dugOutPositions, pos) == false)
					{
						floodFillNextHeads.push_back(pos);
						dugOutPositions.insert(std::ranges::upper_bound(dugOutPositions, pos), pos);
					}
				}

				floodFillCurrentHeads = floodFillNextHeads;
				floodFillNextHeads.clear();
			}

			return dugOutPositions.size();
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

			const u64 digMeters = std::stoi(digMetersStr);
			Assert(digMeters > 0);

			String digColor = Split(Split(Split(digColorStr, ")")[0], "(")[0], "#")[0]; // yes, this is what I did. deal with it.

			ground.Dig(digDir, digMeters);
		}

		u64 result = ground.DigInsideArea();

		return std::to_string(result);
	}

	export String ExecutePart2()
	{
		auto input = OpenInput("day18.txt");

		Ground ground;

		Vec2I diggerPos;
		Vector<Vec2I> dugVertices;
		dugVertices.push_back(diggerPos);

		if constexpr (IsDebug())
		{
			LogFormat("Starting digging at {},{}", diggerPos.X, diggerPos.Y);
		}

		String line;
		while (std::getline(input, line))
		{
			if constexpr (IsDebug())
			{
				Log(line);
			}

			Vector<String> digInstructionStrs = Split(line, " ");
			Assert(digInstructionStrs.size() == 3);

			const String& digColorStr = digInstructionStrs[2];

			String digColor = Split(Split(Split(digColorStr, ")")[0], "(")[0], "#")[0]; // yes, this is what I did. deal with it.
			Assert(digColor.size() == 6);

			const char hexDir = digColor[5];
			Direction digDir = Direction::Invalid;
			switch (hexDir)
			{
				case '3': digDir = Direction::Up; break;
				case '1': digDir = Direction::Down; break;
				case '2': digDir = Direction::Left; break;
				case '0': digDir = Direction::Right; break;
			}
			Assert(digDir != Direction::Invalid);

			digColor.pop_back();

			const u64 digMeters = std::stoi(digColor, 0, 16);
			Assert(digMeters > 0);

			diggerPos += DirToVec2(digDir) * digMeters;
			dugVertices.push_back(diggerPos);

			if constexpr (IsDebug())
			{
				LogFormat("Digging {} meters to {},{}", digMeters, diggerPos.X, diggerPos.Y);
			}
		}

		Vec2I topLeft;
		Vec2I bottomRight;

		for (const Vec2I& pos : dugVertices)
		{
			topLeft.X = std::min(topLeft.X, pos.X);
			topLeft.Y = std::min(topLeft.Y, pos.Y);

			bottomRight.X = std::max(bottomRight.X, pos.X);
			bottomRight.Y = std::max(bottomRight.Y, pos.Y);
		}
		const Vec2I offset = -topLeft;


		constexpr bool outputSVG = IsDebug() || false;
		const s32 strokeWidth = static_cast<u32>(Length(bottomRight - topLeft)) / 500;

		std::ofstream visualizeAreaFile;
		if constexpr (outputSVG)
		{
			visualizeAreaFile.open("output_day18.htm");
			Assert(visualizeAreaFile);
		}

		String svgPositions;

		f64 dugOutAreaSum = 0.;
		Assert(dugVertices.back() == dugVertices.front());
		dugVertices.pop_back();

		// insides are right, but positive kartesian quadrant is oriented y up, thus we are going counter-clockwise by default here

		const Vec2I* lastVertex = &(dugVertices.back());
		u64 circumference = 0;
		for (auto it = dugVertices.begin(); it < dugVertices.end(); ++it)
		{
			const Vec2I& currentVertex = *it;
			const Vec2I currentPos = currentVertex + offset;
			const Vec2I lastPos = *lastVertex + offset;
			const u64 delta = Length(currentPos - lastPos);
			circumference += delta;

			if constexpr (IsDebug())
			{
				LogFormat("{}, {} -> {}, {} (length {})", lastPos.X, lastPos.Y, currentPos.X, currentPos.Y, delta);
			}

			// https://en.wikipedia.org/wiki/Shoelace_formula
			// add 0.5,0.5 to each coordinate to use the center of the hole cubes
			dugOutAreaSum +=
				((lastPos.X + 0.5) * (currentPos.Y + 0.5))
				-
				((currentPos.X + 0.5) * (lastPos.Y + 0.5));

			lastVertex = &currentVertex;

			if constexpr (outputSVG)
			{
				svgPositions.append(StringFormat("{},{} ", currentPos.X, currentPos.Y));
			}
		}

		if constexpr (outputSVG)
		{
			String out = R"(<!DOCTYPE html><html><head><style>>body, html{margin: 0; padding: 0; height: 100%; width: 100%; display: flex; justify-content: center; align-items: center;} svg{display: block; max-width: 100%; max-height: 100vh; height: auto; width: auto;}</style></head>
							)";
			out.append(StringFormat(R"(<body><svg viewBox="{} {} {} {}" preserveAspectratio = "xMidYMid meet">
									)", -strokeWidth, -strokeWidth, bottomRight.X - topLeft.X + strokeWidth * 2, bottomRight.Y - topLeft.Y + strokeWidth * 2));
			out.append(StringFormat(R"("<polygon points =
									{}
									style="fill:grey;stroke:black;stroke-width:{}" /></svg></body></html>
									)", svgPositions, strokeWidth));

			Log("");
			Log(out);
			visualizeAreaFile << out;
			visualizeAreaFile.close();
		}

		f64 verticesArea = std::abs(dugOutAreaSum / 2.);

		// add half of each hole cube (which was outside the polygon), and add 1 for all outside quarters that were not included
		f64 result = verticesArea + 1 + (circumference / 2);

		return std::to_string(static_cast<s64>(result));
	}
}