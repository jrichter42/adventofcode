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
		u32 FloatingSpaces = 0;
		RowContentType Type = RowContentType::Group;

		RowContent* NextSpacer = nullptr;

		template<typename Func>
		void PropagateSpaces(u32 spaces, Func&& onSpacesPropagated, u32 depth = 0)
		{
			if (NextSpacer == nullptr)
			{
				if constexpr (IsDebug())
				{
					LogFormat("{} x {}\n", depth, spaces);
				}
				FloatingSpaces = spaces;
				onSpacesPropagated();
				return;
			}

			for (u32 i = 0; i <= spaces; i++)
			{
				if constexpr (IsDebug())
				{
					LogFormat("{} > {}", depth, i);
				}
				FloatingSpaces = i;
				const u32 remainingSpaces = spaces - i;
				NextSpacer->PropagateSpaces(remainingSpaces, onSpacesPropagated, depth + 1);
			}
		}

		bool CouldBeAt(const Vector<SpringCondition>& row, u32 startIndex) const
		{
			const u32 ownSize = Size + FloatingSpaces;
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
			RowContent* lastSpacer = nullptr;
			for (RowContent& content : playground)
			{
				minRowWidthNeeded += content.Size;
				if (content.Type != RowContentType::Group)
				{
					if (lastSpacer != nullptr)
					{
						lastSpacer->NextSpacer = &content;
					}
					lastSpacer = &content;
				}
			}

			if (rowWidth < minRowWidthNeeded)
			{
				return 0;
			}

			const u32 floatingWidth = rowWidth - minRowWidthNeeded;

			RowContent& firstSpacer = playground[0];
			Assert(firstSpacer.Type == RowContentType::BorderSpacer);

			u64 validPlaygrounds = 0;
			auto testCurrentPlayground = [&]() {
				u32 currentRowIndex = 0;
				for (const RowContent& content : playground)
				{
					const u32 contentSize = content.Size + content.FloatingSpaces;
					const bool contentHappy = content.CouldBeAt(Input_ConditionRecords, currentRowIndex);
					if (contentHappy == false)
					{
						return;
					}
					currentRowIndex += contentSize;

					if (currentRowIndex > rowWidth)
					{
						return;
					}
				}

				validPlaygrounds++;
			};

			if (floatingWidth == 0)
			{
				testCurrentPlayground();
				return validPlaygrounds;
			}

			if constexpr (IsDebug())
			{
				LogFormat("\n\n-> {}", floatingWidth);
			}
			firstSpacer.PropagateSpaces(floatingWidth, testCurrentPlayground);

			return validPlaygrounds;
		}
	};

	struct Worker
	{
		template<typename Func>
		Worker(u32 id, Func&& func)
			: ID(id)
		{
			Future = Promise.get_future();
			LogFormat("Thread {} starting", ID);
			Thread = std::thread(std::forward<decltype(func)>(func), std::move(Promise)); // promise should ideally be moved
		}

		void JoinThread()
		{
			LogFormat("Thread {} waiting to finish", ID);
			Thread.join();
			LogFormat("Thread {} joined", ID);
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

			// we might be fine to reference the same almanac as we only read but better not risk it, this copy shouldn't be relevant
			workers.emplace_back(workerID,
				[row, workerID](std::promise<u64>&& promise) mutable
			{
				const u64 arrangementsCount = row.BruteforceArrangements();

				LogFormat("Thread {} result: {}", workerID, arrangementsCount);
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
				switch (c)
				{
					case '.': row.Input_ConditionRecords.push_back(SpringCondition::Operational); break;
					case '#': row.Input_ConditionRecords.push_back(SpringCondition::Damaged); break;
					case '?': row.Input_ConditionRecords.push_back(SpringCondition::Unknown); break;
					default: Assert(false); break;
				}
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
		return "";
	}
}