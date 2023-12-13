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

		Vector<u32> HorizontalMirrorsAfterCleaning;
		Vector<u32> VerticalMirrorsAfterCleaning;

		void AnalyzeContent()
		{
			Assert(Content.empty() == false);
			Height = NarrowSizeT(Content.size());

			Assert(Content[0].empty() == false);
			Width = NarrowSizeT(Content[0].size());

			using vec2 = std::pair<u32, u32>; // should finally fix this
			struct SmudgeInfo
			{
				SmudgeInfo(const vec2& posBehind, const vec2& posAhead, u32 mirrorPos, bool horizontal)
					: PosBehind(posBehind)
					, PosAhead(posAhead)
				{
					if (horizontal)
					{
						HorizontalMirror = mirrorPos;
					}
					else
					{
						VerticalMirror = mirrorPos;
					}
				}

				vec2 PosBehind;
				vec2 PosAhead;
				u32 HorizontalMirror = u32_MAX;
				u32 VerticalMirror = u32_MAX;

				bool IsHorizontal() const { return HorizontalMirror != u32_MAX; }
			};

			Vector<SmudgeInfo> smudgePositions;

			bool redoHorizontalMirrorsHack = false;
			if (false)
			{
				// probably not needed but I'm too lazy to think this through properly
			//redoHorizontalMirrorAfterVerticalSmudges:
				redoHorizontalMirrorsHack = true;
				HorizontalMirrors.clear();
				HorizontalMirrorsAfterCleaning.clear();
			}

			for (u32 potentialYMirror = 1; potentialYMirror < Height; potentialYMirror++)
			{
				bool cleaningNeeded = false; // lets hope we dont need to track the smudge pos
				const u32 mirroredDepth = std::min<u32>(potentialYMirror, Height - potentialYMirror);
				bool mirrorFound = true;
				for (u32 i = 0; i < mirroredDepth; i++)
				{
					const u32 yBehind = potentialYMirror - i - 1;
					const u32 yAhead = potentialYMirror + i;
					for (u32 x = 0; x < Width; x++)
					{
						if (Content[yBehind][x] != Content[yAhead][x])
						{
							if (!cleaningNeeded)
							{
								smudgePositions.emplace_back(std::make_pair(x, yBehind), std::make_pair(x, yAhead), potentialYMirror, false);
								cleaningNeeded = true;
								continue;
							}
							mirrorFound = false;
							break;
						}
					}
				}

				if (mirrorFound)
				{
					if (!cleaningNeeded)
					{
						HorizontalMirrors.push_back(potentialYMirror);
					}
					else
					{
						HorizontalMirrorsAfterCleaning.push_back(potentialYMirror);
					}
				}
			}

			if (redoHorizontalMirrorsHack)
			{
				return;
			}
			else if (smudgePositions.empty())
			{
				redoHorizontalMirrorsHack = true;
			}

			for (u32 potentialXMirror = 1; potentialXMirror < Width; potentialXMirror++)
			{
				bool cleaningNeeded = false; // lets hope we dont need to track the smudge pos
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
							if (!cleaningNeeded)
							{
								smudgePositions.emplace_back(std::make_pair(xBehind, y), std::make_pair(xAhead, y), potentialXMirror, true);
								cleaningNeeded = true;
								continue;
							}
							mirrorFound = false;
							break;
						}
					}
				}

				if (mirrorFound)
				{
					if (!cleaningNeeded)
					{
						VerticalMirrors.push_back(potentialXMirror);
					}
					else
					{
						VerticalMirrorsAfterCleaning.push_back(potentialXMirror);
					}
				}
			}

			if (redoHorizontalMirrorsHack)
			{
				//goto redoHorizontalMirrorAfterVerticalSmudges;
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
			for (u32 horizontalMirror : pattern.HorizontalMirrorsAfterCleaning)
			{
				result += 100 * horizontalMirror;
			}
			for (u32 verticalMirror : pattern.VerticalMirrorsAfterCleaning)
			{
				result += verticalMirror;
			}
		}

		return std::to_string(result);
	}
}