export module day5;

import common;

export namespace aoc
{
	struct Almanac
	{
		struct CategoryMapping
		{
			struct Range
			{
				Range(u64 startInclusive, u64 count)
					: StartInclusive(startInclusive)
					, Count(count)
				{}

				u64 StartInclusive = 0;
				u64 Count = 0;

				u64 EndInclusive() const { return StartInclusive + Count; }

				bool InRange(u64 index) const
				{
					return index >= StartInclusive
						&& index <= EndInclusive();
				}

				u64 MapTo(const Range& destinationRange, u64 index) const
				{
					Assert(destinationRange.Count == Count, "Source and Destination do not match");
					Assert(InRange(index), "Source range failed");
					const u64 toDestinationOffset = destinationRange.StartInclusive - StartInclusive;
					const u64 result = index + toDestinationOffset;
					Assert(destinationRange.InRange(result), "Destination range failed");
					return result;
				}
			};

			struct RangeMapping : protected std::pair<Range, Range>
			{
				RangeMapping(u64 sourceStartInclusive, u64 destinationStartInclusive, u64 count)
					: std::pair<Range, Range>(Range(sourceStartInclusive, count), Range(destinationStartInclusive, count))
				{
					LogFormat("{} {} {}", destinationStartInclusive, sourceStartInclusive, count);
				}

				const Range& Source() const { return first; }
				const Range& Destination() const { return second; }
			};

			String Name;
			Vector<RangeMapping> RangeMappings;

			u64 SourceToDestination(u64 sourceIndex) const
			{
				for (const RangeMapping& mapping : RangeMappings)
				{
					if (mapping.Source().InRange(sourceIndex))
					{
						return mapping.Source().MapTo(mapping.Destination(), sourceIndex);
					}
				}
				return sourceIndex;
			}

			u64 DestinationToSource(u64 desinationIndex) const
			{
				for (const RangeMapping& mapping : RangeMappings)
				{
					if (mapping.Destination().InRange(desinationIndex))
					{
						return mapping.Destination().MapTo(mapping.Source(), desinationIndex);
					}
				}
				return desinationIndex;
			}
		};

		Vector<u64> Seeds;
		Vector<u64> SeedLocations;
		Vector<CategoryMapping> CategoryMappings;
	};

	export String ExecutePart1()
	{
		auto input = OpenInput("day5.txt");

		Almanac almanac;

		String line;
		std::getline(input, line);

		// Seeds
		Vector<String> seedInfo = Split(line, ": ");
		Vector<String> seedStrings = Split(seedInfo[1], " ");

		Assert(seedInfo[0] == "seeds");
		std::for_each(seedStrings.begin() + 1, seedStrings.end(), [&](const String& seedStr)
			{
				u64 seed = ToUnsigned(std::stoll(seedStr));
				almanac.Seeds.push_back(std::move(seed));
			});

		// Category Mappings
		Almanac::CategoryMapping pendingMapping;
		bool firstMapping = true;
		while (std::getline(input, line))
		{
			if (line.empty())
			{
				if (!firstMapping)
				{
					almanac.CategoryMappings.push_back(pendingMapping);
				}
				firstMapping = false;

				if (!std::getline(input, line))
				{
					break;
				}
				Assert(line.empty() == false);

				pendingMapping = Almanac::CategoryMapping();
				pendingMapping.Name = Split(line, " ")[0];
				continue;
			}

			Vector<String> mappingValueStrings = Split(line, " ");
			Assert(mappingValueStrings.size() == 3);

			const u64 destinationStart = ToUnsigned(std::stoll(mappingValueStrings[0]));
			const u64 sourceStart = ToUnsigned(std::stoll(mappingValueStrings[1]));
			const u64 count = ToUnsigned(std::stoll(mappingValueStrings[2]));

			LogFormat("{}\n{} {} {}", line, destinationStart, sourceStart, count);

			pendingMapping.RangeMappings.emplace_back(sourceStart, destinationStart, count);
		}
		almanac.CategoryMappings.push_back(pendingMapping);

		u64 lowestSeedLocation = u64_MAX;
		for (u64 seed : almanac.Seeds)
		{
			u64 currentIndex = seed;
			for (const Almanac::CategoryMapping& mapping : almanac.CategoryMappings)
			{
				currentIndex = mapping.SourceToDestination(currentIndex);
			}

			lowestSeedLocation = std::min(currentIndex, lowestSeedLocation);
		}

		return std::to_string(lowestSeedLocation);
	}

	export String ExecutePart2()
	{
		return "";
	}
}