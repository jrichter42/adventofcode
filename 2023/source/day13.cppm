export module day13;

import common;

export namespace aoc
{
	struct Pattern
	{
		u32 Width = 0;
		u32 Height = 0;
		Vector<Vector<bool>> Content; // Content[y][x]
		Vector<u32> HorizontalMirrors;
		Vector<u32> VerticalMirrors;

		void AnalyzeContent()
		{
			Assert(Content.empty() == false);
			Height = NarrowSizeT(Content.size());

			Assert(Content[0].empty() == false);
			Width = NarrowSizeT(Content[0].size());

			for (u32 potentialYMirror = 1; potentialYMirror < Height; potentialYMirror++)
			{
				const u32 mirroredDepth = std::min<u32>(potentialYMirror, Height - potentialYMirror);
				bool mirrorFound = true;
				for (u32 i = 0; i < mirroredDepth; i++)
				{
					const u32 yBehind = potentialYMirror - i - 1;
					const u32 yAhead = potentialYMirror + i;
					if (Content[yBehind] != Content[yAhead])
					{
						mirrorFound = false;
						break;
					}
				}

				if (mirrorFound)
				{
					HorizontalMirrors.push_back(potentialYMirror);
				}
			}

			for (u32 potentialXMirror = 1; potentialXMirror < Width; potentialXMirror++)
			{
				const u32 mirroredDepth = std::min<u32>(potentialXMirror, Width - potentialXMirror);
				bool mirrorFound = true;
				for (u32 i = 0; i < mirroredDepth; i++)
				{
					for (u32 y = 0; y < Height; y++)
					{
						const u32 xBehind = potentialXMirror - i - 1;
						const u32 xAhead = potentialXMirror + i;
						if (Content[y][xBehind] != Content[y][xAhead])
						{
							mirrorFound = false;
							break;
						}
					}
				}

				if (mirrorFound)
				{
					VerticalMirrors.push_back(potentialXMirror);
				}
			}
		}
	};

	export String ExecutePart1()
	{
		auto input = OpenInput("day13.txt");

		Vector<Pattern> patterns(1);
		Pattern* currentPattern = &patterns.back();

		String line;
		while (std::getline(input, line))
		{
			if (line.empty())
			{
				currentPattern->AnalyzeContent();
				currentPattern = &patterns.emplace_back();
				continue;
			}

			Vector<bool>& currentRow = currentPattern->Content.emplace_back();

			for (const char c : line)
			{
				Assert(c == '#' || c == '.');
				currentRow.push_back(c == '#');
			}
		}
		currentPattern->AnalyzeContent();

		u32 result = 0;
		for (const Pattern& pattern : patterns)
		{
			for (u32 horizontalMirror : pattern.HorizontalMirrors)
			{
				result += 100 * horizontalMirror;
			}
			for (u32 verticalMirror : pattern.VerticalMirrors)
			{
				result += verticalMirror;
			}
		}

		return std::to_string(result);
	}

	export String ExecutePart2()
	{
		return "";
	}
}