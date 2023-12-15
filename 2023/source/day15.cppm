export module day15;

import common;

export namespace aoc
{
	u32 Hash(const String& str)
	{
		u32 result = 0;
		for (char c : str)
		{
			u8 ascii = static_cast<u32>(c);
			result += ascii;
			result *= 17;
			result %= 256;
		}
		return result;
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

	export String ExecutePart2()
	{
		return "";
	}
}