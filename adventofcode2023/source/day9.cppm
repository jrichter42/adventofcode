export module day9;

import common;

export namespace aoc
{
	Vector<s32> GetDeltas(const Vector<s32>& values)
	{
		if (values.size() < 2)
		{
			return Vector<s32>();
		}

		Vector<s32> result;
		result.reserve(values.size() - 1);

		for (u32 i = 0; i < values.size() - 1; i++)
		{
			const s32& lhs = values[i];
			const s32& rhs = values[i + 1];
			const s32 delta = rhs - lhs;
			result.push_back(std::move(delta));
		}

		return result;
	}

	s32 CalculateNextValueInSequence(const Vector<s32>& values)
	{
		const s32 lastValue = values.back();

		const bool isConstantSequence = std::adjacent_find(values.begin(), values.end(), std::not_equal_to<>()) == values.end();
		if (isConstantSequence)
		{
			return lastValue;
		}

		Vector<s32> derivativeSequence = GetDeltas(values);
		const s32 derivativeSequenceNextValue = CalculateNextValueInSequence(derivativeSequence);

		return lastValue + derivativeSequenceNextValue;
	}

	export String ExecutePart1()
	{
		auto input = OpenInput("day9.txt");

		s64 extrapolatedValuesSum = 0;
		String line;
		while (std::getline(input, line))
		{
			Vector<String> valueStrings = Split(line, " ");
			Vector<s32> values;
			for (const String& valueString : valueStrings)
			{
				const s32 value = std::stoi(valueString);
				values.push_back(value);
			}

			const s32 nextSequenceValue = CalculateNextValueInSequence(values);
			extrapolatedValuesSum += nextSequenceValue;
		}

		return std::to_string(extrapolatedValuesSum);
	}

	export String ExecutePart2()
	{
		auto input = OpenInput("day9.txt");

		s64 extrapolatedValuesSum = 0;
		String line;
		while (std::getline(input, line))
		{
			Vector<String> valueStrings = Split(line, " ");
			Vector<s32> values;
			for (const String& valueString : valueStrings)
			{
				const s32 value = std::stoi(valueString);
				values.push_back(value);
			}

			std::reverse(values.begin(), values.end());

			const s32 nextSequenceValue = CalculateNextValueInSequence(values);
			extrapolatedValuesSum += nextSequenceValue;
		}

		return std::to_string(extrapolatedValuesSum);
	}
}