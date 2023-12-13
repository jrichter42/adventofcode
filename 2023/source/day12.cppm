export module day12;

import common;

export namespace aoc
{
	enum SpringCondition
	{
		Invalid,
		Operational,
		Damaged,
		Unknown
	};

	SpringCondition CharToSpringCondition(char c)
	{
		switch (c)
		{
			case '.': return SpringCondition::Operational;
			case '#': return SpringCondition::Damaged;
			case '?': return SpringCondition::Unknown;
		}
		Assert(false);
		return SpringCondition::Invalid;
	}

	char SpringConditionToChar(SpringCondition condition)
	{
		switch (condition)
		{
			case SpringCondition::Unknown: return '?';
			case SpringCondition::Damaged: return '#';
			case SpringCondition::Operational: return '.';
		}
		Assert(false);
		return ' ';
	}

	enum class RowContentType
	{
		Group,
		MiddleSpacer,
		BorderSpacer
	};

	struct RowContent
	{
		RowContent(u32 size, RowContentType type = RowContentType::Group)
			: Size(size)
			, Type(type)
		{}

		u32 Size = 0;
		RowContentType Type = RowContentType::Group;

		RowContent* NextContent = nullptr;

		struct FindSubArrangementsCacheEntry
		{
			FindSubArrangementsCacheEntry(u32 rowPos, u32 spaces, u64 result)
				: CurrentRowPos(rowPos)
				, RemainingFloatingSpaces(spaces)
				, Result(result)
			{}

			constexpr bool operator<(const FindSubArrangementsCacheEntry& rhs)
			{
				if (CurrentRowPos != rhs.CurrentRowPos)
				{
					return CurrentRowPos < rhs.CurrentRowPos;
				}
				return RemainingFloatingSpaces < rhs.RemainingFloatingSpaces;
			}

			constexpr bool operator==(const FindSubArrangementsCacheEntry& other)
			{
				return CurrentRowPos == other.CurrentRowPos
					&& RemainingFloatingSpaces == other.RemainingFloatingSpaces;
			}

			u32 CurrentRowPos = 0;
			u32 RemainingFloatingSpaces = 0;
			u64 Result = 0;
		};

		Vector<FindSubArrangementsCacheEntry> FindSubArrangementsCache;

		u64 PropagateSpacesAndFindValidArrangements(const Vector<SpringCondition>& row, u32 currentRowPos, u32 spaces, u32 recursionDepth = 0)
		{
			if constexpr (IsDebug() && false)
			{
				if (currentRowPos < row.size())
				{
					LogFormat("{} {}", currentRowPos, SpringConditionToChar(row[currentRowPos]));
				}
				else
				{
					LogFormat("{} {}", currentRowPos, "out of bounds");
				}
			}

			auto cacheIt = std::find(FindSubArrangementsCache.begin(), FindSubArrangementsCache.end(), FindSubArrangementsCacheEntry(currentRowPos, spaces, 0));
			if (cacheIt != FindSubArrangementsCache.end())
			{
				const FindSubArrangementsCacheEntry& cacheEntry = *cacheIt;
				if constexpr (IsDebug())
				{
					String depthVisualizationStr; for (u32 debugI = 0; debugI < recursionDepth; debugI++) { depthVisualizationStr += '>'; }
					LogFormat("{} {} _ #{}", depthVisualizationStr, spaces, cacheEntry.Result);
				}
				return cacheEntry.Result;
			}

			u64 numValidArrangements = 0;
			if (Type == RowContentType::Group)
			{
				// GROUP
				if (CouldBeAt(row, currentRowPos, 0))
				{
					if constexpr (IsDebug())
					{
						String depthVisualizationStr; for (u32 debugI = 0; debugI < recursionDepth; debugI++) { depthVisualizationStr += '>'; }
						LogFormat("{} {} - {}", depthVisualizationStr, spaces, spaces);
					}
					const u64 numValidSubArrangements = NextContent->PropagateSpacesAndFindValidArrangements(row, currentRowPos + Size, spaces, recursionDepth + 1);
					numValidArrangements += numValidSubArrangements;
				}
			}
			else
			{
				// SPACER

				if (NextContent == nullptr)
				{
					const u32 myFloatingSpaces = spaces;
					if (CouldBeAt(row, currentRowPos, myFloatingSpaces))
					{
						if constexpr (IsDebug())
						{
							String depthVisualizationStr; for (u32 debugI = 0; debugI < recursionDepth; debugI++) { depthVisualizationStr += '>'; }
							LogFormat("{} {} < #1 |", depthVisualizationStr, spaces);
						}
						return 1; // bypassing cache is fine, this is cheap enough
					}
					if constexpr (IsDebug())
					{
						String depthVisualizationStr; for (u32 debugI = 0; debugI < recursionDepth; debugI++) { depthVisualizationStr += '>'; }
						LogFormat("{} {} < #0 |", depthVisualizationStr, spaces);
					}
					return 0; // bypassing cache is fine, this is cheap enough
				}

				for (u32 i = 0; i <= spaces; i++)
				{
					const u32 ownFloatingSpaces = i;
					const u32 remainingSpaces = spaces - ownFloatingSpaces;
					if (CouldBeAt(row, currentRowPos, ownFloatingSpaces))
					{
						const u32 ownSize = Size + ownFloatingSpaces;

						if constexpr (IsDebug())
						{
							String depthVisualizationStr; for (u32 debugI = 0; debugI < recursionDepth; debugI++) { depthVisualizationStr += '>'; }
							LogFormat("{} {} > {}", depthVisualizationStr, spaces, remainingSpaces);
						}
						const u64 numValidSubArrangements = NextContent->PropagateSpacesAndFindValidArrangements(row, currentRowPos + ownSize, remainingSpaces, recursionDepth + 1);
						numValidArrangements += numValidSubArrangements;
					}
				}
			}

			// GROUP & SPACER
			if constexpr (IsDebug())
			{
				String depthVisualizationStr; for (u32 debugI = 0; debugI < recursionDepth; debugI++) { depthVisualizationStr += '>'; }
				LogFormat("{} {} < #{}", depthVisualizationStr, spaces, numValidArrangements);
			}

			FindSubArrangementsCache.emplace_back(currentRowPos, spaces, numValidArrangements);
			return numValidArrangements;
		}

		bool CouldBeAt(const Vector<SpringCondition>& row, u32 startIndex, u32 floatingSpaces) const
		{
			const u32 ownSize = Size + floatingSpaces;
			if (ownSize == 0)
			{
				return true;
			}

			const u32 endIndexExclusive = startIndex + ownSize;
			if (endIndexExclusive > row.size())
			{
				return false;
			}

			for (u32 i = startIndex; i < endIndexExclusive; i++)
			{
				const SpringCondition condition = row[i];
				if (Type == RowContentType::Group)
				{
					if (condition == SpringCondition::Operational)
					{
						return false;
					}
					continue;
				}

				// spacer
				if (condition == SpringCondition::Damaged)
				{
					return false;
				}
			}

			return true;
		}
	};

	struct SpringRow
	{
		Vector<SpringCondition> Input_ConditionRecords;
		Vector<u32> Input_GroupSizes;

		u32 Width() const { return NarrowSizeT(Input_ConditionRecords.size()); }

		u64 BruteforceArrangements()
		{
			const u32 rowWidth = Width();

			const u32 groupCount = NarrowSizeT(Input_GroupSizes.size());
			const u32 spacerCount = groupCount + 1;
			const u32 rowContentCount = groupCount + spacerCount;
			Assert(rowContentCount % 2 == 1);

			Vector<RowContent> playground;

			auto groupSizeIt = Input_GroupSizes.begin();
			for (u32 i = 0; i < rowContentCount; i++)
			{
				if (i == 0 || i == rowContentCount - 1)
				{
					// border spacer
					playground.emplace_back(0, RowContentType::BorderSpacer);
					continue;
				}

				if (i % 2 == 0)
				{
					// middle spacer
					playground.emplace_back(1, RowContentType::MiddleSpacer);
					continue;
				}

				// actual group
				playground.emplace_back(*groupSizeIt);
				groupSizeIt++;
			}

			u32 minRowWidthNeeded = 0;
			RowContent* lastContent = nullptr;
			for (RowContent& content : playground)
			{
				minRowWidthNeeded += content.Size;
				if (lastContent != nullptr)
				{
					lastContent->NextContent = &content;
				}
				lastContent = &content;
			}

			if (rowWidth < minRowWidthNeeded)
			{
				return 0;
			}

			const u32 floatingWidth = rowWidth - minRowWidthNeeded;

			RowContent& firstContent = playground[0];
			Assert(firstContent.Type == RowContentType::BorderSpacer);

			if constexpr (IsDebug())
			{
				String conditionsDebugStr;
				for (SpringCondition condition : Input_ConditionRecords)
				{
					conditionsDebugStr += SpringConditionToChar(condition);
				}

				String groupsDebugStr;
				bool first = true;
				for (u32 groupSize : Input_GroupSizes)
				{
					if (!first)
					{
						groupsDebugStr += ',';
					}
					first = false;
					groupsDebugStr += std::to_string(groupSize);
				}

				LogFormat("\n{} {} -> {} ", conditionsDebugStr, groupsDebugStr, floatingWidth);
			}
			const u64 numValidArrangements = firstContent.PropagateSpacesAndFindValidArrangements(Input_ConditionRecords, 0, floatingWidth);

			return numValidArrangements;
		}
	};

	struct Worker
	{
		template<typename Func>
		Worker(u32 id, Func&& func)
			: ID(id)
		{
			Future = Promise.get_future();
			if constexpr (IsDebug())
			{
				LogFormat("Thread {} starting", ID);
			}
			Thread = std::thread(std::forward<decltype(func)>(func), std::move(Promise)); // promise should ideally be moved
		}

		void JoinThread()
		{
			if constexpr (IsDebug())
			{
				LogFormat("Thread {} waiting to finish", ID);
			}
			Thread.join();
			if constexpr (IsDebug())
			{
				LogFormat("Thread {} joined", ID);
			}
		}

		u64 GetResult()
		{
			return Future.get();
		}

		u32 ID = 0;
		std::thread Thread;
		std::promise<u64> Promise;
		std::future<u64> Future;
	};

	u64 BruteforceArrangementsMT(Vector<SpringRow>& springRows)
	{
		Vector<Worker> workers;

		u32 workerID = 0;
		for (u32 rowIndex = 0; rowIndex < springRows.size(); rowIndex += 2)
		{
			SpringRow& row = springRows[rowIndex];

			// we might be fine to reference the same data as we only read but better not risk it, this copy shouldn't be relevant
			workers.emplace_back(workerID,
				[row, workerID](std::promise<u64>&& promise) mutable
			{
				const u64 arrangementsCount = row.BruteforceArrangements();

				if constexpr (IsDebug() || false)
				{
					LogFormat("Thread {} result: {}", workerID, arrangementsCount);
				}
				promise.set_value(arrangementsCount);
			});

			workerID++;
		}

		for (Worker& worker : workers)
		{
			worker.JoinThread();
		}

		u64 resultSum = 0;
		for (Worker& worker : workers)
		{
			const u64 workerResult = worker.GetResult();
			resultSum += workerResult;
		}

		return resultSum;
	}

	export String ExecutePart1()
	{
		auto input = OpenInput("day12.txt");

		Vector<SpringRow> springRows;

		String line;
		while (std::getline(input, line))
		{
			Vector<String> formatStrings = Split(line, " ");
			Assert(formatStrings.size() == 2);

			SpringRow row;
			for (char c : formatStrings[0])
			{
				const SpringCondition condition = CharToSpringCondition(c);
				row.Input_ConditionRecords.push_back(condition);
			}

			Vector<String> contiguousGroupStrings = Split(formatStrings[1], ",");
			for (const String& str : contiguousGroupStrings)
			{
				u32 groupSize = std::stoi(str);
				row.Input_GroupSizes.push_back(groupSize);
			}

			springRows.push_back(std::move(row));
		}

		u64 result = 0;

		constexpr bool multithread = false;

		if constexpr (multithread)
		{
			result = BruteforceArrangementsMT(springRows);
		}
		else
		{
			for (SpringRow& row : springRows)
			{
				result += row.BruteforceArrangements();
			}
		}

		return std::to_string(result);
	}

	export String ExecutePart2()
	{
		auto input = OpenInput("day12.txt");

		Vector<SpringRow> springRows;

		String line;
		while (std::getline(input, line))
		{
			Vector<String> formatStrings = Split(line, " ");
			Assert(formatStrings.size() == 2);

			SpringRow row;

			Vector<String> contiguousGroupStrings = Split(formatStrings[1], ",");

			bool first = true;
			for (u32 unfoldIdx = 0; unfoldIdx < 5; unfoldIdx++)
			{
				if (!first)
				{
					row.Input_ConditionRecords.push_back(SpringCondition::Unknown);
				}
				first = false;

				for (char c : formatStrings[0])
				{
					const SpringCondition condition = CharToSpringCondition(c);
					row.Input_ConditionRecords.push_back(condition);
				}

				for (const String& str : contiguousGroupStrings)
				{
					u32 groupSize = std::stoi(str);
					row.Input_GroupSizes.push_back(groupSize);
				}
			}

			springRows.push_back(std::move(row));
		}

		u64 result = 0;

		constexpr bool multithread = false;

		if constexpr (multithread)
		{
			result = BruteforceArrangementsMT(springRows);
		}
		else
		{
			for (SpringRow& row : springRows)
			{
				result += row.BruteforceArrangements();
			}
		}

		return std::to_string(result);
	}
}