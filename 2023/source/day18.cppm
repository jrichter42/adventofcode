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
			LogFormat("Digging {} meters", meters);
			for (u64 i = 0; i < meters; ++i)
			{
				DiggerPos += DirToVec2(dir);

				DugOutVolumes.push_back(std::make_pair(DiggerPos, dir));
			}

			std::sort(DugOutVolumes.begin(), DugOutVolumes.end());
			DugOutVolumes.erase(std::unique(DugOutVolumes.begin(), DugOutVolumes.end() ), DugOutVolumes.end());
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

			LogFormat("Dug out {}. Starting digging inside.", dugOutPositions.size());

			auto tryDig = [&](const Vec2I& pos, Vector<Vec2I>& log) {
				if (std::ranges::binary_search(dugOutPositions, pos) == false)
				{
					log.push_back(pos);
					dugOutPositions.insert(std::ranges::upper_bound(dugOutPositions, pos ), pos);
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

			LogFormat("Dug out {}. Starting flood digging.", dugOutPositions.size());

			// flood fill/dig from all InsideLoop until Loop or InsideLoop

			Vector<Vec2I> floodFillCurrentHeads = writtenInsidePositions;
			Vector<Vec2I> floodFillNextHeads;
			Vec2I pos;
			while (floodFillCurrentHeads.empty() == false)
			{
				//LogFormat("Flood dig heads: {} Dug out: {}", floodFillCurrentHeads.size(), dugOutPositions.size());
				for (const Vec2I& currentHead : floodFillCurrentHeads)
				{
					pos = currentHead + DirToVec2(Direction::Left);
					if (std::ranges::binary_search(dugOutPositions, pos) == false)
					{
						floodFillNextHeads.push_back(pos);
						dugOutPositions.insert(std::ranges::upper_bound(dugOutPositions, pos ), pos);
					}

					pos = currentHead + DirToVec2(Direction::Up);
					if (std::ranges::binary_search(dugOutPositions, pos) == false)
					{
						floodFillNextHeads.push_back(pos);
						dugOutPositions.insert(std::ranges::upper_bound(dugOutPositions, pos ), pos);
					}

					pos = currentHead + DirToVec2(Direction::Right);
					if (std::ranges::binary_search(dugOutPositions, pos) == false)
					{
						floodFillNextHeads.push_back(pos);
						dugOutPositions.insert(std::ranges::upper_bound(dugOutPositions, pos ), pos);
					}

					pos = currentHead + DirToVec2(Direction::Down);
					if (std::ranges::binary_search(dugOutPositions, pos) == false)
					{
						floodFillNextHeads.push_back(pos);
						dugOutPositions.insert(std::ranges::upper_bound(dugOutPositions, pos ), pos);
					}
				}

				floodFillCurrentHeads = floodFillNextHeads;
				floodFillNextHeads.clear();
			}

			return dugOutPositions.size();
		}

		void WriteDiggingToFile()
		{
			// https://stackoverflow.com/questions/2654480/writing-bmp-image-in-pure-c-c-without-other-libraries

			constexpr bool debugBMP = false;
			constexpr bool debugContent = false;

			Log("Writing digging to file output_day18.bmp");
			Log("Flood fill and count pixels in pixel-based image editor");

			Vec2I topLeft;
			Vec2I bottomRight;

			Vector<Vec2I> dugOutPositions;
			for (const Ground::GroundVolume& volume : DugOutVolumes)
			{
				const Vec2I& pos = volume.first;

				topLeft.X = std::min(topLeft.X, pos.X);
				topLeft.Y = std::min(topLeft.Y, pos.Y);

				bottomRight.X = std::max(bottomRight.X, pos.X);
				bottomRight.Y = std::max(bottomRight.Y, pos.Y);

				dugOutPositions.push_back(pos);
			}
			std::ranges::sort(dugOutPositions);

			const s64 w = bottomRight.X - topLeft.X + 8;
			const s64 h = bottomRight.Y - topLeft.Y + 8;
			const Vec2I offset = Vec2I(4, 4) - topLeft;

			FILE* f;
			unsigned char* img = nullptr;
			const s64 rowSize = (w + 31) / 32 * 4; // Row size padded to 4-byte boundary
			s64 filesize = 54 + 8 + rowSize * h;  // Add 8 bytes for the color table

			if constexpr (debugBMP)
			{
				LogFormat("Image dimensions (w x h): {} x {}", std::to_string(w), std::to_string(h));
				LogFormat("Row size (bytes): {}", std::to_string(rowSize));
				LogFormat("File size (bytes): {}", std::to_string(filesize));
			}

			img = (unsigned char*)std::malloc(rowSize * h);
			std::memset(img, 0, rowSize * h);

			for (const Vec2I& pos : dugOutPositions)
			{
				Vec2I imgPos = pos + offset;
				imgPos.Y = (h - 1) - imgPos.Y;

				if (imgPos.X < 0 || imgPos.X >= w || imgPos.Y < 0 || imgPos.Y >= h)
				{
					LogFormat("Position out of bounds: {},{}", imgPos.X, imgPos.Y);
					continue;
				}

				s32 bitIndex = static_cast<s32>(imgPos.X % 8); // Bit position within the byte
				s32 byteIndex = static_cast<s32>(imgPos.X / 8); // Byte position within the row
				img[byteIndex + imgPos.Y * rowSize] |= (1 << (7 - bitIndex));
			}


			unsigned char bmpfileheader[14] = { 'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0 };
			unsigned char bmpinfoheader[40] = { 40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0 };

			bmpfileheader[2] = (unsigned char)(filesize);
			bmpfileheader[3] = (unsigned char)(filesize >> 8);
			bmpfileheader[4] = (unsigned char)(filesize >> 16);
			bmpfileheader[5] = (unsigned char)(filesize >> 24);

			bmpinfoheader[4] = static_cast<unsigned char>(w);
			bmpinfoheader[5] = (unsigned char)(w >> 8);
			bmpinfoheader[6] = (unsigned char)(w >> 16);
			bmpinfoheader[7] = (unsigned char)(w >> 24);
			bmpinfoheader[8] = static_cast<unsigned char>(h);
			bmpinfoheader[9] = (unsigned char)(h >> 8);
			bmpinfoheader[10] = (unsigned char)(h >> 16);
			bmpinfoheader[11] = (unsigned char)(h >> 24);
			bmpinfoheader[14] = 1; // 1 bit monochrome

			if constexpr (debugBMP)
			{
				LogFormat("BMP File Header: ");
				LogFormat("File type: {}{}", (char)bmpfileheader[0], (char)bmpfileheader[1]); // Should be 'B', 'M'
				LogFormat("File size: {}", *((u32*)&bmpfileheader[2]));     // File size in bytes
				LogFormat("Data offset: {}", *((u32*)&bmpfileheader[10]));  // Should be 54 + color table size

				LogFormat("BMP Info Header: ");
				LogFormat("Header size: {}", *((u32*)&bmpinfoheader[0]));   // Should be 40
				LogFormat("Image width: {}", *((s32*)&bmpinfoheader[4]));    // Image width
				LogFormat("Image height: {}", *((s32*)&bmpinfoheader[8]));   // Image height
				LogFormat("Planes: {}", *((u16*)&bmpinfoheader[12]));       // Should be 1
				LogFormat("Bit count: {}", *((u16*)&bmpinfoheader[14]));    // Should be 1 for monochrome
				LogFormat("Image size: {}", *((u32*)&bmpinfoheader[20]));   // Can be 0 for BI_RGB bitmaps
			}

			errno_t returnValue = fopen_s(&f, "output_day18.bmp", "wb");
			Assert(returnValue == 0);
			fwrite(bmpfileheader, 1, 14, f);
			fwrite(bmpinfoheader, 1, 40, f);

			unsigned char colorTable[8] = { 0,0,0,0, 255,255,255,0 }; // Black and White
			fwrite(colorTable, 1, 8, f);

			if constexpr (debugContent)
			{
				for (int i = 0; i < h; i++)
				{
					unsigned char* row = img + (rowSize * (h - i - 1));
					std::string rowContent;
					for (int j = 0; j < rowSize; j++)
					{
						rowContent += std::bitset<8>(row[j]).to_string() + " ";
					}
					LogFormat("Row {} content: {}", h - i, rowContent);
				}
			}

			for (int i = 0; i < h; i++)
			{
				fwrite(img + (rowSize * (h - i - 1)), 1, rowSize, f);
			}

			std::free(img);
			fclose(f);
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
		LogFormat("Starting digging at {},{}", diggerPos.X, diggerPos.Y);

		String line;
		while (std::getline(input, line))
		{
			Log(line);

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

			//ground.Dig(digDir, digMeters);

			diggerPos += DirToVec2(digDir) * digMeters;
			LogFormat("Digging {} meters to {},{}", digMeters, diggerPos.X, diggerPos.Y);
			dugVertices.push_back(diggerPos);
		}

		// outsource inside digging to image-based flood fill algorithms
		//ground.WriteDiggingToFile(); // scratch that, area of example is too big already


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


		constexpr bool outputSVG = true;
		const u32 strokeWidth = static_cast<u32>(Length(bottomRight - topLeft)) / 200;
		std::ofstream visualizeAreaFile;
		visualizeAreaFile.open("output_day18.htm");
		Assert(visualizeAreaFile);

		if constexpr (outputSVG)
		{
			Log("");
			String out = R"(<!DOCTYPE html><html><head><style>)"
						 R"(body, html)"
						 R"({margin: 0; padding: 0; height: 100%; width: 100%; display: flex; justify-content: center; align-items: center;} )"
						 R"(svg)"
						 R"({display: block; max-width: 100%; max-height: 100vh; height: auto; width: auto;})"
						 R"(</style></head><body>)";
			out.append("\n");
			out.append(R"(<svg viewBox=")");
			out.append(StringFormat("0 0 {} {}", bottomRight.X - topLeft.X, bottomRight.Y - topLeft.Y));
			out.append(R"(" preserveAspectratio="xMidYMid meet">)"
					   R"(<polygon points = ")");
			Log(out);
			visualizeAreaFile << out;
		}

		String svgPositions;

		f64 dugOutAreaSum = 0.;
		Assert(dugVertices.back() == dugVertices.front());
		dugVertices.pop_back();

		Log("");

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
			LogFormat("{}, {} -> {}, {} (length {})", lastPos.X, lastPos.Y, currentPos.X, currentPos.Y, delta);

			// https://en.wikipedia.org/wiki/Shoelace_formula , add 0.5,0.5 to each coordinate to use the center of the hole cubes
			dugOutAreaSum +=
				(
					(lastPos.X + 0.5)
					*
					(currentPos.Y + 0.5)
				)
				-
				(
					(currentPos.X + 0.5)
					*
					(lastPos.Y + 0.5)
				);

			lastVertex = &currentVertex;

			if constexpr (outputSVG)
			{
				svgPositions.append(StringFormat("{},{} ", currentPos.X, currentPos.Y));
			}
		}

		Log("");

		if constexpr (outputSVG)
		{
			Log(svgPositions);
			visualizeAreaFile << svgPositions;

			String out = R"(" style="fill:none;stroke:black;stroke-width:)";
			out.append(StringFormat(R"({}" /></svg>)", strokeWidth));
			out.append("\n</body></html>\n");
			Log(out);
			visualizeAreaFile << out;
		}
		visualizeAreaFile.close();

		f64 result = std::abs(dugOutAreaSum / 2.);
		// add half of each hole cube (which was outside the polygon), and add 1 for all outside quarters that were not included
		const s64 integerResult = static_cast<s64>(result) + 1 + (circumference / 2);

		return std::to_string(integerResult);
	}
}