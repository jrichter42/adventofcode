export module day15;

import common;

export namespace aoc
{
	u8 Hash(const String& str)
	{
		u32 result = 0;
		for (char c : str)
		{
			result += static_cast<u32>(c);
			result *= 17;
			result %= 256;
		}
		Assert(result <= u8_MAX);
		return static_cast<u8>(result);
	}

	export String ExecutePart1()
	{
		auto input = OpenInput("day15.txt");

		u32 result = 0;

		String str;

		auto runHash = [&]() {
			const u32 hashResult = Hash(str);
			result += hashResult;
		};

		char c;
		u32 debugPos = 0;
		while (Read<char>(input, c))
		{
			debugPos++;
			if (c == ',')
			{
				runHash();
				str.clear();
				continue;
			}

			str.push_back(c);
		}
		runHash();

		return std::to_string(result);
	}

	struct Lens
	{
		String Label;
		u8 FocalLength = u8_MAX;

		constexpr bool operator==(const String& label) const
		{
			return Label == label;
		}

		constexpr auto operator<=>(const String& label) const
		{
			return Label <=> label;
		}
	};

	struct Box
	{
		Vector<Lens> InstalledLenses;

		void InsertLens(Lens&& lens)
		{
			auto existingLensIt = std::find(InstalledLenses.begin(), InstalledLenses.end(), lens.Label);
			if (existingLensIt != InstalledLenses.end())
			{
				// who cares
				existingLensIt->Label = std::move(lens.Label);
				existingLensIt->FocalLength = lens.FocalLength;
				return;
			}

			InstalledLenses.push_back(std::forward<Lens>(lens));
		}

		void RemoveLens(const String& lensLabel)
		{
			InstalledLenses.erase(std::remove(InstalledLenses.begin(), InstalledLenses.end(), lensLabel), InstalledLenses.end());
		}
	};

	export String ExecutePart2()
	{
		auto input = OpenInput("day15.txt");

		Array<Box, 256> boxes;

		String lensLabel;
		u8 focalLength = u8_MAX;

		auto runStep = [&]() {
			const u8 boxIndex = Hash(lensLabel);
			if (focalLength != u8_MAX)
			{
				Lens lens;
				lens.Label = lensLabel;
				lens.FocalLength = focalLength;
				boxes[boxIndex].InsertLens(std::move(lens));
			}
			else
			{
				boxes[boxIndex].RemoveLens(lensLabel);
			}
		};

		char c;
		u32 debugPos = 0;
		while (Read<char>(input, c))
		{
			debugPos++;
			if (c == '=')
			{
				continue;
			}

			if (c == '-')
			{
				// remove lens
				continue;
			}

			if (c >= '0' && c <= '9')
			{
				// insert lens
				focalLength = static_cast<u8>(c - '0');
				continue;
			}

			if (c == ',')
			{
				runStep();
				lensLabel.clear();
				focalLength = u8_MAX;
				continue;
			}

			lensLabel.push_back(c);
		}
		runStep();

		u32 result = 0;

		for (u32 boxIdx = 0; boxIdx < 256; boxIdx++)
		{
			const Box& box = boxes[boxIdx];
			u32 boxResult = 0;
			for (u32 lensIdx = 0; lensIdx < box.InstalledLenses.size(); lensIdx++)
			{
				const Lens& lens = box.InstalledLenses[lensIdx];
				const u32 lensFocusingPower = (boxIdx + 1) * (lensIdx + 1) * lens.FocalLength;
				boxResult += lensFocusingPower;
			}
			result += boxResult;
		}

		return std::to_string(result);
	}
}