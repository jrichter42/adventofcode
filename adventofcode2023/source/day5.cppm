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

				u64 EndExclusive() const { return StartInclusive + Count; }

				bool InRange(const u64& index) const
				{
					return index >= StartInclusive
						&& index < EndExclusive();
				}

				u64 MapTo(const Range& destinationRange, const u64& index) const
				{
					//Assert(destinationRange.Count == Count, "Source and Destination do not match");
					//Assert(InRange(index), "Source range failed");
					const u64 toDestinationOffset = destinationRange.StartInclusive - StartInclusive;
					const u64 result = index + toDestinationOffset;
					//Assert(destinationRange.InRange(result), "Destination range failed");
					return result;
				}

				u64 CalculateLowestSeedMT(const Almanac& almanac, const u32 workerID)
				{
					//LogFormat("Starting seeds start {} count {}", StartInclusive, Count);

					u64 lowestSeedLocation = u64_MAX;

					u32 seedOffset = 0;
					u64 currentIndex = 0;
					u64 nextCustomRangeIndex = 0;
					bool seedToSoilMapping = true;
					while (seedOffset < Count)
					{
						currentIndex = StartInclusive + seedOffset;
						seedOffset++;
						seedToSoilMapping = true;
						for (const Almanac::CategoryMapping& mapping : almanac.CategoryMappings)
						{
							if (seedToSoilMapping)
							{
								if (currentIndex >= nextCustomRangeIndex)
								{
									// Can't bypass first level mapping anymore
									currentIndex = mapping.SourceToDestination(currentIndex, &nextCustomRangeIndex);
								}

								seedToSoilMapping = false;
								continue;
							}

							currentIndex = mapping.SourceToDestination(currentIndex, nullptr);
						}

						lowestSeedLocation = std::min(currentIndex, lowestSeedLocation);
					}

					//LogFormat("Finished seeds start {} count {}", StartInclusive, Count);

					return lowestSeedLocation;
				}
			};

			struct RangeMapping : public std::pair<Range, Range>
			{
				RangeMapping(u64 sourceStartInclusive, u64 destinationStartInclusive, u64 count)
					: std::pair<Range, Range>(Range(sourceStartInclusive, count), Range(destinationStartInclusive, count))
				{
					//LogFormat("{} {} {}", destinationStartInclusive, sourceStartInclusive, count);
				}

				bool operator<(const RangeMapping& rhs) const
				{
					return first.StartInclusive < rhs.first.StartInclusive;
				}

				bool operator<(const u64& rhsValue) const
				{
					return first.EndExclusive() <= rhsValue;
				}

				bool operator==(const u64& value)
				{
					return first.InRange(value);
				}

				//const Range& Source() const { return first; }
				//const Range& Destination() const { return second; }
			};

			String Name;
			Vector<RangeMapping> RangeMappings;

			u64 SourceToDestination(const u64& sourceIndex, u64* nextCustomRangeIndex = nullptr) const
			{
				auto it = std::lower_bound(RangeMappings.begin(), RangeMappings.end(), sourceIndex);

				if (it != RangeMappings.end())
				{
					const RangeMapping& mapping = *it;
					if (mapping.first.InRange(sourceIndex))
					{
						return mapping.first.MapTo(mapping.second, sourceIndex);
					}
					else
					{
						if (nextCustomRangeIndex)
						{
							*nextCustomRangeIndex = mapping.first.StartInclusive;
						}
					}
				}

				/*for (const RangeMapping& mapping : RangeMappings)
				{
					if (mapping.first.InRange(sourceIndex))
					{
						return mapping.first.MapTo(mapping.second, sourceIndex);
					}
				}*/

				return sourceIndex;
			}

			/*u64 DestinationToSource(u64 desinationIndex) const
			{
				for (const RangeMapping& mapping : RangeMappings)
				{
					if (mapping.Destination().InRange(desinationIndex))
					{
						return mapping.Destination().MapTo(mapping.Source(), desinationIndex);
					}
				}
				return desinationIndex;
			}*/

			void Optimize()
			{
				std::sort(RangeMappings.begin(), RangeMappings.end());
			}
		};

		void Optimize()
		{
			for (CategoryMapping& mapping : CategoryMappings)
			{
				mapping.Optimize();
			}
		}

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

		u64 CalculateLowestSeedRangeLocationMT()
		{
			Vector<Worker> workers;

			u32 workerID = 0;
			for (u32 seedIdx = 0; seedIdx < SeedNumbers.size(); seedIdx += 2)
			{
				const u64 seedStart = SeedNumbers[seedIdx];
				const u64 seedCount = SeedNumbers[seedIdx + 1];

				// we might be fine to reference the same almanac as we only read but better not risk it, this copy shouldn't be relevant
				workers.emplace_back(workerID,
					[*this, seedStart, seedCount, workerID](std::promise<u64>&& promise)
					{
						CategoryMapping::Range seedRange(seedStart, seedCount);

						const u64 rangeResult = seedRange.CalculateLowestSeedMT(*this, workerID);

						LogFormat("Thread {} result: {}", workerID, rangeResult);
						promise.set_value(rangeResult);
					});

				workerID++;
			}

			for (Worker& worker : workers)
			{
				worker.JoinThread();
			}

			u64 lowestSeedLocation = u64_MAX;
			for (Worker& worker : workers)
			{
				const u64 workerResult = worker.GetResult();
				lowestSeedLocation = std::min(workerResult, lowestSeedLocation);
			}

			return lowestSeedLocation;
		}

		Vector<u64> SeedNumbers;
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
		std::for_each(seedStrings.begin(), seedStrings.end(), [&](const String& seedStr)
			{
				u64 seed = ToUnsigned(std::stoll(seedStr));
				almanac.SeedNumbers.push_back(std::move(seed));
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

			//LogFormat("{}\n{} {} {}", line, destinationStart, sourceStart, count);

			pendingMapping.RangeMappings.emplace_back(sourceStart, destinationStart, count);
		}
		almanac.CategoryMappings.push_back(pendingMapping);
		almanac.Optimize();

		u64 lowestSeedLocation = u64_MAX;
		for (u64 seed : almanac.SeedNumbers)
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
		auto input = OpenInput("day5.txt");

		Almanac almanac;

		String line;
		std::getline(input, line);

		// Seeds
		Vector<String> seedInfo = Split(line, ": ");
		Vector<String> seedStrings = Split(seedInfo[1], " ");

		Assert(seedInfo[0] == "seeds");
		std::for_each(seedStrings.begin(), seedStrings.end(), [&](const String& seedStr)
		{
			u64 seed = ToUnsigned(std::stoll(seedStr));
			almanac.SeedNumbers.push_back(std::move(seed));
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

			pendingMapping.RangeMappings.emplace_back(sourceStart, destinationStart, count);
		}
		almanac.CategoryMappings.push_back(pendingMapping);
		almanac.Optimize();

		// I'm too lazy to restructure into a more cache-friendly version (e.g. map all indices in one category, then move on to mapping all through the next layer, etc)
		// so we just parallelize the data traversal and let it be the CPU heatsink's problem
		u64 result = almanac.CalculateLowestSeedRangeLocationMT();

		return std::to_string(result);
	}
}