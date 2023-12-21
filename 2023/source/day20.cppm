export module day20;

import common;

export namespace aoc
{
	enum class ModuleType : u8
	{
		Invalid,
		FlipFlop,
		Conjunction,
		Broadcast,
		Button
	};

	struct Module;

	struct PulseInstruction
	{
		PulseInstruction(u8 from, u8 to, bool isHighPulse)
			: PulseInstruction(
				from,
				[](u8 to) -> u64 {
					u64 prePackedTargets = u64_MAX;
					prePackedTargets &= ~(static_cast<u64>(0x7F) << 7); // clear
					prePackedTargets |= static_cast<u64>(to & 0x7F) << 7; // set
					return prePackedTargets;
				}(to)
				, isHighPulse
			)
		{}

		PulseInstruction(u8 from, u64 prePackedTargets, bool isHighPulse)
		{
			Data = 0;
			Data |= static_cast<u64>(from & 0x7F);
			Data |= (prePackedTargets & 0x00FFFFFFFFFFFFFF) << 7; // 56 bits: 8 targets, each 7 bits
			Data |= static_cast<u64>(isHighPulse) << 63;
		}

		// returns u64 which holds 56 bits: 8 x 7 bit targets
		[[nodiscard]] u64 Unpack(u8& from, bool& isHighPulse)
		{
			from = Data & 0x7F;
			isHighPulse = (Data >> 63) & 0x01;
			const u64 targets = (Data >> 7) & 0x00FFFFFFFFFFFFFF;
			return targets;
		}

	private:
		// 7 bit (0-6) from index
		// 8 x 7 bit (7-62) target indices
		// 1 bit (63) isHighPulse
		u64 Data = u64_MAX;
	};

	struct Module
	{
		constexpr bool operator==(u8 idx) const
		{
			return MapIdx == idx;
		}

		constexpr auto operator<=>(u8 idx) const
		{
			return MapIdx <=> idx;
		}

		constexpr bool operator==(const Module& other) const
		{
			return MapIdx == other.MapIdx;
		}

		constexpr auto operator<=>(const Module& other) const
		{
			return MapIdx <=> other.MapIdx;
		}

		__forceinline void Pulse(const Module& from, bool pulseIsHigh, std::queue<PulseInstruction>& outPulses)
		{
			constexpr bool debug = IsDebug() && false;
			switch (Type)
			{
				case ModuleType::FlipFlop:
				{
					if (pulseIsHigh)
					{
						if constexpr (debug)
						{
							LogFormat("%{} -> ignore", MapIdx);
						}
						return;
					}
					FlipFlopState = !FlipFlopState;
					if constexpr (debug)
					{
						LogFormat("%{} -> {}", MapIdx, FlipFlopState ? "on" : "off");
					}

					PulseAllTargets(FlipFlopState, outPulses);
				} return;
				case ModuleType::Conjunction:
				{
					if (pulseIsHigh == false)
					{
						ConjunctionState.erase(std::remove(ConjunctionState.begin(), ConjunctionState.end(), from.MapIdx), ConjunctionState.end());
					}
					else
					{
						auto it = find (ConjunctionState.begin(), ConjunctionState.end(), from.MapIdx);
						if (it == ConjunctionState.end())
						{
							ConjunctionState.push_back(from.MapIdx);
						}
					}

					const bool pulseOutHigh = ConjunctionState.empty();

					if constexpr (debug)
					{
						LogFormat("&{} -> {}", MapIdx, pulseOutHigh ? "high" : "low");
					}

					PulseAllTargets(pulseOutHigh, outPulses);
				} return;
				case ModuleType::Broadcast:
				{
					if constexpr (debug)
					{
						LogFormat("< -> {}", pulseIsHigh ? "high" : "low");
					}

					PulseAllTargets(pulseIsHigh, outPulses);
				} return;
				case ModuleType::Button:
				{
					if constexpr (debug)
					{
						LogFormat("O -> {}", pulseIsHigh ? "high" : "low");
					}

					PulseAllTargets(pulseIsHigh, outPulses);
				} return;
			}
		}

		__forceinline void PulseAllTargets(bool pulseIsHigh, std::queue<PulseInstruction>& outPulses)
		{
			outPulses.emplace(MapIdx, Targets, pulseIsHigh);
		}

		void AddTarget(u8 value)
		{
			for (u32 i = 0; i < 8; ++i)
			{
				const u8 currentValue = (Targets >> (i * 7)) & 0x7F;
				if (currentValue == 0x7F)
				{
					// clear
					Targets &= ~(static_cast<u64>(0x7F) << (i * 7));
					// set
					Targets |= static_cast<u64>(value) << (i * 7);
					return;
				}
			}
			Assert(false); // more than 8 slots needed
		}

		//String Name;
		u8 MapIdx = u8_MAX;
		ModuleType Type = ModuleType::Invalid;

	private:
		// 8 x 7 bit targets
		u64 Targets = u64_MAX; // 0xFFFFFFFFFFFFFFFF, every u8: 0xFF

	public:
		// last pulse low modules
		Vector<u8> ConjunctionState;
		bool FlipFlopState = false;
	};

	export String ExecutePart1()
	{
		auto input = OpenInput("day20.txt");

		Array<Module, 59> modules;
		Array<std::pair<String, String>, 59> moduleTargets;

		// parse modules

		u8 parsedModuleIdx = 0;
		String line;
		while (std::getline(input, line))
		{
			Vector<String> typeNameTargetsStr = Split(line, " -> ");
			const String& typeNameStr = typeNameTargetsStr[0];
			const String& targetsStr = typeNameTargetsStr[1];

			ModuleType type = ModuleType::Broadcast;
			switch (typeNameStr[0])
			{
				case '%': type = ModuleType::FlipFlop; break;
				case '&': type = ModuleType::Conjunction; break;
			}

			String name;
			if (type != ModuleType::Broadcast)
			{
				name = typeNameStr.substr(1, typeNameStr.length()-1);
			}
			else
			{
				name = typeNameStr;
			}

			Module& mdl = modules[parsedModuleIdx];
			mdl.Type = type;

			moduleTargets[parsedModuleIdx] = std::make_pair(name, targetsStr);

			++parsedModuleIdx;
		}

		Module& button = modules[parsedModuleIdx];
		button.Type = ModuleType::Button;
		moduleTargets[parsedModuleIdx] = std::make_pair("button", "broadcaster");

		++parsedModuleIdx;

		// write map locations
		for (u8 i = 0; i < modules.size(); ++i)
		{
			modules[i].MapIdx = i;
		}

		// link module targets via to indices (now available)
		Assert(modules.size() == moduleTargets.size());
		for (u32 i = 0; i < moduleTargets.size(); ++i)
		{
			const auto& moduleTargetPair = moduleTargets[i];

			Module& mdl = modules[i];

			const String& targetsStr = moduleTargetPair.second;
			Vector<String> targetStrings = Split(targetsStr, ", ");
			for (const String& targetStr : targetStrings)
			{
				auto targetIt = std::find_if(moduleTargets.begin(), moduleTargets.end(), [&](const auto& pair) { return pair.first == targetStr; });

				if (targetIt == moduleTargets.end())
				{
					// no Module for target, pass nullptr sentinel value
					mdl.AddTarget(0x6F);
				}
				else
				{
					const u8 targetIdx = NarrowSizeT<u8>(std::distance(moduleTargets.begin(), targetIt));
					mdl.AddTarget(targetIdx);
				}
			}
		}

		// pulse

		auto broadcasterIt = std::find_if(moduleTargets.begin(), moduleTargets.end(), [](const auto& pair) { return pair.first == String("broadcaster"); });
		Assert(broadcasterIt != moduleTargets.end());
		u8 broadcasterIdx = NarrowSizeT<u8>(std::distance(moduleTargets.begin(), broadcasterIt));
		Module& broadcaster = modules[broadcasterIdx];

		constexpr bool initialPulseHigh = false;
		PulseInstruction initialInstruction(button.MapIdx, broadcaster.MapIdx, initialPulseHigh);

		Vector<Array<Module, 59>> moduleStateHistory;
		Vector<std::pair<u64, u64>> pulsedStateHistory;

		u64 lowPulsedCount = 0;
		u64 highPulsedCount = 0;
		for (u32 i = 0; i < 1000; ++i)
		{
			for (auto modulesIt = moduleStateHistory.rbegin(); modulesIt != moduleStateHistory.rend(); ++modulesIt)
			{
				// detect loop

				bool matches = true;
				const Array<Module, 59>& modulesState = *modulesIt;
				for (u32 moduleIdx = 0; moduleIdx < modulesState.size(); ++moduleIdx)
				{
					const Module& mdl = modulesState[moduleIdx];
					const Module& currentMdl = modules[moduleIdx];

					if (mdl.MapIdx != currentMdl.MapIdx
						|| mdl.FlipFlopState != currentMdl.FlipFlopState
						|| mdl.ConjunctionState != currentMdl.ConjunctionState)
					{
						matches = false;
						break;
					}
				}

				if (matches)
				{
					// loop found
					const u32 loopSize = NarrowSizeT(std::distance(moduleStateHistory.rbegin(), modulesIt)) + 1;

					const u32 remainingRuns = 1000 - i;
					const u32 remainingRunsRemainder = remainingRuns % loopSize;

					const u32 loopsSkippedCount = (remainingRuns - remainingRunsRemainder) / loopSize;

					auto pulsedIt = pulsedStateHistory.rbegin() += loopSize - 1;
					const auto& pulsedPair = *pulsedIt;

					lowPulsedCount += (lowPulsedCount - pulsedPair.first) * loopsSkippedCount;
					highPulsedCount += (highPulsedCount - pulsedPair.second) * loopsSkippedCount;

					i += loopSize * loopsSkippedCount;
					Assert(1000 - i == remainingRunsRemainder);
					LogFormat("Loop found at button press {}", loopSize);
					break;
				}
			}
			if (i >= 1000)
			{
				break;
			}

			moduleStateHistory.push_back(modules);
			pulsedStateHistory.push_back(std::make_pair(lowPulsedCount, highPulsedCount));

			// press button

			if constexpr (IsDebug() || true)
			{
				LogFormat("Press button {}, pulsed {} high, {} low", i, highPulsedCount, lowPulsedCount);
			}

			std::queue<PulseInstruction> pulseInstructions;
			pulseInstructions.push(initialInstruction);
			std::queue<PulseInstruction> nextPulseInstructions;
			while (pulseInstructions.empty() == false)
			{
				// pulse current layer

				if constexpr (IsDebug() || true)
				{
					LogFormat("Pulsing {}", pulseInstructions.size());
				}

				while (pulseInstructions.empty() == false)
				{
					PulseInstruction& instruction = pulseInstructions.front();
					pulseInstructions.pop();

					if constexpr (IsDebug() || false)
					{
						//LogFormat("Pulse {}: {} -> {}", instruction.HighPulse ? "high" : "low ", instruction.From->Name, instruction.To ? instruction.To->Name : "nullptr");
						//LogFormat("{} -{}> {}", instruction.GetFrom()->Name, instruction.HighPulse ? "high" : "low", instruction.GetTo() ? instruction.GetTo()->Name : "nullptr");
					}

					u8 moduleFromIdx = u8_MAX;
					bool highPulse = false;
					const u64 packedModuleToIndices = instruction.Unpack(moduleFromIdx, highPulse);
					Assert(moduleFromIdx < modules.size());

					u8 targetCount = 0;
					for (u32 targetIdx = 0; targetIdx < 8; ++targetIdx)
					{
						const u8 moduleToIdx = (packedModuleToIndices >> (targetIdx * 7)) & 0x7F;
						if (moduleToIdx == 0x7F)
						{
							break;
						}

						++targetCount;

						// 0x6F if target == nullptr
						if (moduleToIdx < modules.size())
						{
							Module& mdlFrom = modules[moduleFromIdx];
							Module& mdlTo = modules[moduleToIdx];

							mdlTo.Pulse(mdlFrom, highPulse, nextPulseInstructions);
						}
					}

					if (highPulse)
					{
						highPulsedCount += targetCount;
					}
					else
					{
						lowPulsedCount += targetCount;
					}
				}

				pulseInstructions.swap(nextPulseInstructions);
			}
		}

		const u64 result = highPulsedCount * lowPulsedCount;

		return std::to_string(result);
	}

	export String ExecutePart2()
	{
		return "";
	}
}