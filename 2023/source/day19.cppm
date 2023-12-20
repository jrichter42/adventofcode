export module day19;

import common;

export namespace aoc
{
	struct Part
	{
		using MemberT = s64;

		Part() = default;

		explicit Part(MemberT initValue)
			: x(initValue)
			, m(initValue)
			, a(initValue)
			, s(initValue)
		{}

		String ToString() const
		{
			return StringFormat("{},{},{},{}", x, m, a, s);
		}

		u64 GetRating() const
		{
			return x + m + a + s;
		}

		MemberT x = 0;
		MemberT m = 0;
		MemberT a = 0;
		MemberT s = 0;

		constexpr bool operator==(const Part&) const = default;
		constexpr auto operator<=>(const Part&) const = default;
	};

	enum class PartMemberSelector
	{
		Invalid, x, m, a, s
	};

	PartMemberSelector CharToPartMemberSelector(char c)
	{
		switch (c)
		{
			case 'x': return PartMemberSelector::x;
			case 'm': return PartMemberSelector::m;
			case 'a': return PartMemberSelector::a;
			case 's': return PartMemberSelector::s;
		}
		Assert(false);
		return PartMemberSelector::Invalid;
	}

	char PartMemberSelectorToChar(PartMemberSelector selector)
	{
		switch (selector)
		{
			case PartMemberSelector::x: return 'x';
			case PartMemberSelector::m: return 'm';
			case PartMemberSelector::a: return 'a';
			case PartMemberSelector::s: return 's';
		}
		Assert(false);
		return ' ';
	}

	struct Rule
	{
		using Compare = std::function<bool(Part::MemberT, Part::MemberT)>;
		Rule(PartMemberSelector member, Compare comparator, Part::MemberT valueToCompareTo, String workflowOnSuccess)
			: PartMember(member)
			, Comparator(comparator)
			, ValueToCompareTo(valueToCompareTo)
			, WorkflowOnSuccess(workflowOnSuccess)
		{
			Greater = Comparator(1, 0);
		}

		String ToString() const
		{
			return StringFormat("({} {} {}) -> {}", PartMemberSelectorToChar(PartMember), Greater ? ">" : "<", ValueToCompareTo, WorkflowOnSuccess);
		}

		PartMemberSelector PartMember = PartMemberSelector::Invalid;
		Compare Comparator = std::less<Part::MemberT>();
		Part::MemberT ValueToCompareTo;
		String WorkflowOnSuccess;
		bool Greater = false;

		bool operator()(const Part& value) const
		{
			switch (PartMember)
			{
				case PartMemberSelector::x: return Comparator(value.x, ValueToCompareTo);
				case PartMemberSelector::m: return Comparator(value.m, ValueToCompareTo);
				case PartMemberSelector::a: return Comparator(value.a, ValueToCompareTo);
				case PartMemberSelector::s: return Comparator(value.s, ValueToCompareTo);
			}
			Assert(false);
			return false;
		}

		void SetMinMaxPartsToFitResult(Part& partMin, Part& partMax, bool succeed) const
		{
			Part::MemberT minValue = 0;
			Part::MemberT maxValue = 0;
			if (Greater)
			{
				if (succeed)
				{
					minValue = ValueToCompareTo + 1;
					maxValue = 4000;
				}
				else
				{
					minValue = 1;
					maxValue = ValueToCompareTo;
				}
			}
			else
			{
				if (succeed)
				{
					minValue = 1;
					maxValue = ValueToCompareTo - 1;
				}
				else
				{
					minValue = ValueToCompareTo;
					maxValue = 4000;
				}
			}

			switch (PartMember)
			{
				case PartMemberSelector::x: partMax.x = std::min(partMax.x, maxValue); partMin.x = std::max(partMin.x, minValue); break;
				case PartMemberSelector::m: partMax.m = std::min(partMax.m, maxValue); partMin.m = std::max(partMin.m, minValue); break;
				case PartMemberSelector::a: partMax.a = std::min(partMax.a, maxValue); partMin.a = std::max(partMin.a, minValue); break;
				case PartMemberSelector::s: partMax.s = std::min(partMax.s, maxValue); partMin.s = std::max(partMin.s, minValue); break;
				default: Assert(false); break;
			}
		}
	};

	struct PartConstraint
	{
		// self will fail rule, return value will succeed rule
		PartConstraint FitThroughRule(const Rule& rule)
		{
			PartConstraint branchedResult = *this;

			// fail rule
			rule.SetMinMaxPartsToFitResult(MinValues, MaxValues, false);
			// succeed rule
			rule.SetMinMaxPartsToFitResult(branchedResult.MinValues, branchedResult.MaxValues, true);

			return branchedResult;
		}

		void ConstrainFurther(const PartConstraint& constraint)
		{
			MinValues.x = std::max(MinValues.x, constraint.MinValues.x);
			MaxValues.x = std::min(MaxValues.x, constraint.MaxValues.x);

			MinValues.m = std::max(MinValues.m, constraint.MinValues.m);
			MaxValues.m = std::min(MaxValues.m, constraint.MaxValues.m);

			MinValues.a = std::max(MinValues.a, constraint.MinValues.a);
			MaxValues.a = std::min(MaxValues.a, constraint.MaxValues.a);

			MinValues.s = std::max(MinValues.s, constraint.MinValues.s);
			MaxValues.s = std::min(MaxValues.s, constraint.MaxValues.s);
		}

		String ToString() const
		{
			return StringFormat("x:{}-{}, m:{}-{}, a:{}-{}, s:{}-{}", MinValues.x, MaxValues.x, MinValues.m, MaxValues.m, MinValues.a, MaxValues.a, MinValues.s, MaxValues.s);
		}

		u64 Combinations() const
		{
			return (1 + MaxValues.x - MinValues.x)
				* (1 + MaxValues.m - MinValues.m)
				* (1 + MaxValues.a - MinValues.a)
				* (1 + MaxValues.s - MinValues.s);
		}

		constexpr bool operator==(const PartConstraint&) const = default;
		constexpr auto operator<=>(const PartConstraint&) const = default;

		Part MinValues = Part(1);
		Part MaxValues = Part(4000);
	};

	struct Workflow
	{
		constexpr bool operator==(const Workflow& other) const
		{
			return Name == other.Name;
		}

		constexpr auto operator<=>(const Workflow& other) const
		{
			return Name <=> other.Name;
		}

		constexpr bool operator==(const String& value) const
		{
			return Name == value;
		}

		constexpr auto operator<=>(const String& value) const
		{
			return Name <=> value;
		}

		String Execute(const Part& part)
		{
			for (const Rule& rule : Rules)
			{
				const bool result = rule(part);

				if constexpr (IsDebug())
				{
					LogFormat("Rule {} {} {} -> {} <- {}",
							  PartMemberSelectorToChar(rule.PartMember),
							  rule.Greater ? ">" : "<",
							  rule.ValueToCompareTo,
							  rule.WorkflowOnSuccess, result ? "success" : "failed"
					);
				}

				if (result)
				{
					return rule.WorkflowOnSuccess;
				}
			}
			return Fallback;
		}

		String Name;
		Vector<Rule> Rules;
		String Fallback;

		void DetermineConstraints()
		{
			// start unconstrained
			PartConstraint failedConstraint;
			for (const Rule& rule : Rules)
			{
				PartConstraint passedConstraint = failedConstraint.FitThroughRule(rule);
				ConstraintsToPassFailRules.emplace_back(make_tuple(passedConstraint, rule.WorkflowOnSuccess, failedConstraint));

				if constexpr (IsDebug())
				{
					LogFormat("Workflow {}, Rule {}\nPass {}\nFail {}\n", Name, rule.ToString(), passedConstraint.ToString(), failedConstraint.ToString());
				}
			}
		}

		// takes constraint potentially narrower already, returns constraints and related targets which are reachable through this workflow
		void ExecuteOnConstraints(const PartConstraint& partConstraint, Vector<std::pair<PartConstraint, String>>& out) const
		{
			PartConstraint currentConstraintAlongRuleChain = partConstraint;

			for (const auto& tuple : ConstraintsToPassFailRules)
			{
				PartConstraint narrowedConstraintToPass = std::get<0>(tuple);
				const String& targetOnPass = std::get<1>(tuple);

				narrowedConstraintToPass.ConstrainFurther(currentConstraintAlongRuleChain);

				out.emplace_back(make_pair(std::move(narrowedConstraintToPass), targetOnPass));

				const PartConstraint& constraintToFail = std::get<2>(tuple);
				currentConstraintAlongRuleChain.ConstrainFurther(constraintToFail);
			}

			out.emplace_back(make_pair(std::move(currentConstraintAlongRuleChain), Fallback));
		}

		// constraint to pass, target on pass, constraint to fail
		Vector<std::tuple<PartConstraint, String, PartConstraint>> ConstraintsToPassFailRules;
	};


	export String ExecutePart1()
	{
		auto input = OpenInput("day19.txt");

		Vector<Workflow> workflows;
		Vector<Part> parts;

		bool parseParts = false;
		String line;
		while (std::getline(input, line))
		{
			if (line.empty())
			{
				parseParts = true;
				continue;
			}

			if (!parseParts)
			{
				// Workflow
				Vector<String> nameRulesStr = Split(line, "{");
				const String& workflowName = nameRulesStr[0];
				Vector<String> ruleStrings = Split(Split(nameRulesStr[1], "}")[0], ",");

				Workflow& workflow = workflows.emplace_back();
				workflow.Name = workflowName;

				// Rules
				for (const String& ruleStr : ruleStrings)
				{
					Vector<String> ruleResultStr = Split(ruleStr, ":");
					if (ruleResultStr.size() == 1)
					{
						// Fallback
						Assert(workflow.Fallback.empty());
						workflow.Fallback = ruleResultStr[0];
					}
					else
					{
						// Rule
						// Expression
						bool greater = false;
						Vector<String> ruleExpression = Split(ruleResultStr[0], "<");
						if (ruleExpression.size() == 1)
						{
							ruleExpression = Split(ruleResultStr[0], ">");
							greater = true;
						}
						Assert(ruleExpression.size() == 2);

						const PartMemberSelector memberSelector = CharToPartMemberSelector(ruleExpression[0][0]);
						Part::MemberT valueToCompareTo = static_cast<Part::MemberT>(std::stoi(ruleExpression[1]));
						// On Success
						const String& workflowOnSuccess = ruleResultStr[1];
						if (greater)
						{
							workflow.Rules.emplace_back(memberSelector, std::greater<Part::MemberT>(), valueToCompareTo, workflowOnSuccess);
						}
						else
						{
							workflow.Rules.emplace_back(memberSelector, std::less<Part::MemberT>(), valueToCompareTo, workflowOnSuccess);
						}
					}
				}
			}
			else
			{
				// Part
				Vector<String> partMemberStrings = Split(Split(Split(line, "{")[0], "}")[0], ",");

				Part& part = parts.emplace_back();
				for (const String& partMemberStr : partMemberStrings)
				{
					Vector<String> memberValueStrings = Split(partMemberStr, "=");

					Part::MemberT value = static_cast<Part::MemberT>(std::stoi(memberValueStrings[1]));
					switch (memberValueStrings[0][0])
					{
						case 'x': part.x = value; break;
						case 'm': part.m = value; break;
						case 'a': part.a = value; break;
						case 's': part.s = value; break;
						default: Assert(false); break;
					}
				}
			}

		}
		if constexpr (IsDebug() == false)
		{
			std::ranges::sort(workflows);
		}

		u64 acceptedRatingSum = 0;

		auto it = std::find(workflows.begin(), workflows.end(), "in");
		Assert(it != workflows.end());
		for (const Part& part : parts)
		{
			if constexpr (IsDebug())
			{
				LogFormat("Part {}, {}, {}, {}", part.x, part.m, part.a, part.s);
			}
			Workflow* currentWorkflow = &(*it);

			while (true)
			{
				if constexpr (IsDebug())
				{
					LogFormat("Workflow {}, Rules: {}, Fallback: {}", currentWorkflow->Name, currentWorkflow->Rules.size(), currentWorkflow->Fallback);
				}

				String result = currentWorkflow->Execute(part);

				if (result == "R")
				{
					if constexpr (IsDebug())
					{
						Log("Rejected");
					}
					break;
				}
				else if (result == "A")
				{
					if constexpr (IsDebug())
					{
						LogFormat("Acceped: {}", part.GetRating());
					}
					acceptedRatingSum += part.GetRating();
					break;
				}

				auto nextWorkflowIt = std::find(workflows.begin(), workflows.end(), result);
				Assert(nextWorkflowIt != workflows.end());
				currentWorkflow = &(*nextWorkflowIt);
			}
		}

		return std::to_string(acceptedRatingSum);
	}

	export String ExecutePart2()
	{
		auto input = OpenInput("day19.txt");

		Vector<Workflow> workflows;

		String line;
		while (std::getline(input, line))
		{
			if (line.empty())
			{
				break;
			}

			// Workflow
			Vector<String> nameRulesStr = Split(line, "{");
			const String& workflowName = nameRulesStr[0];
			Vector<String> ruleStrings = Split(Split(nameRulesStr[1], "}")[0], ",");

			Workflow& workflow = workflows.emplace_back();
			workflow.Name = workflowName;

			// Rules
			for (const String& ruleStr : ruleStrings)
			{
				Vector<String> ruleResultStr = Split(ruleStr, ":");
				if (ruleResultStr.size() == 1)
				{
					// Fallback
					Assert(workflow.Fallback.empty());
					workflow.Fallback = ruleResultStr[0];
				}
				else
				{
					// Rule
					// Expression
					bool greater = false;
					Vector<String> ruleExpression = Split(ruleResultStr[0], "<");
					if (ruleExpression.size() == 1)
					{
						ruleExpression = Split(ruleResultStr[0], ">");
						greater = true;
					}
					Assert(ruleExpression.size() == 2);

					const PartMemberSelector memberSelector = CharToPartMemberSelector(ruleExpression[0][0]);
					Part::MemberT valueToCompareTo = static_cast<Part::MemberT>(std::stoi(ruleExpression[1]));
					// On Success
					const String& workflowOnSuccess = ruleResultStr[1];
					if (greater)
					{
						workflow.Rules.emplace_back(memberSelector, std::greater<Part::MemberT>(), valueToCompareTo, workflowOnSuccess);
					}
					else
					{
						workflow.Rules.emplace_back(memberSelector, std::less<Part::MemberT>(), valueToCompareTo, workflowOnSuccess);
					}
				}
			}
		}
		if constexpr (IsDebug() == false)
		{
			std::ranges::sort(workflows);
		}

		for (Workflow& workflow : workflows)
		{
			workflow.DetermineConstraints();
		}

		// start with most loose constraint possible
		Vector<std::pair<PartConstraint, String>> currentWorkflows { std::make_pair(PartConstraint(), "in") };
		Vector<std::pair<PartConstraint, String>> nextWorkflows;

		Vector<PartConstraint> acceptedConstraints;
		Vector<PartConstraint> rejectedConstraints;
		while (currentWorkflows.size() > 0)
		{
			if constexpr (IsDebug())
			{
				LogFormat("Constraint heads: {}", currentWorkflows.size());
			}
			for (const auto& currentWfPair : currentWorkflows)
			{
				const PartConstraint& constraint = currentWfPair.first;
				const String& workflowTarget = currentWfPair.second;

				if constexpr (IsDebug())
				{
					LogFormat("Constraint {} -> {}", constraint.ToString(), workflowTarget);
				}

				if (workflowTarget == "R")
				{
					if constexpr (IsDebug())
					{
						Log("Rejected");
					}
					rejectedConstraints.push_back(constraint);
					continue;
				}
				else if (workflowTarget == "A")
				{
					if constexpr (IsDebug())
					{
						LogFormat("Accepted: {}", constraint.MaxValues.GetRating() - constraint.MinValues.GetRating());
					}
					acceptedConstraints.push_back(constraint);
					continue;
				}

				auto currentWorkflowIt = std::find(workflows.begin(), workflows.end(), workflowTarget);
				Assert(currentWorkflowIt != workflows.end());
				const Workflow& currentWorkflow = *currentWorkflowIt;

				if constexpr (IsDebug())
				{
					LogFormat("Workflow {}, Rules: {}, Fallback: {}", currentWorkflow.Name, currentWorkflow.Rules.size(), currentWorkflow.Fallback);
				}

				currentWorkflow.ExecuteOnConstraints(constraint, nextWorkflows);
			}

			currentWorkflows = nextWorkflows;
			nextWorkflows.clear();
		}

		struct XmasTesseract
		{
			using VecT = Vec4T<Part::MemberT>;
			XmasTesseract(const Part& xmasFrom, const Part& xmasTo)
				: Position(std::min(xmasFrom.x, xmasTo.x),
						   std::min(xmasFrom.m, xmasTo.m),
						   std::min(xmasFrom.a, xmasTo.a),
						   std::min(xmasFrom.s, xmasTo.s))
				// each value should take up space, including last one
				, Scale(static_cast<VecT::DataType>(std::abs(xmasTo.x - xmasFrom.x + 1)),
						static_cast<VecT::DataType>(std::abs(xmasTo.m - xmasFrom.m + 1)),
						static_cast<VecT::DataType>(std::abs(xmasTo.a - xmasFrom.a + 1)),
						static_cast<VecT::DataType>(std::abs(xmasTo.s - xmasFrom.s + 1)))
			{}

			String ToString() const
			{
				return StringFormat("{}-{},{}-{},{}-{},{}-{}",
									Position.X, Position.X + Scale.X,
									Position.Y, Position.Y + Scale.Y,
									Position.Z, Position.Z + Scale.Z,
									Position.W, Position.W + Scale.W);
			}

			VecT Center() const
			{
				return Position + Scale * 0.5f;
			}

			u64 Volume() const
			{
				return std::abs(Scale.X * Scale.Y * Scale.Z * Scale.W);
			}

			u64 Overlap(const XmasTesseract& other) const
			{
				VecT centerToCenter = other.Center() - Center();
				centerToCenter.X = static_cast<VecT::DataType>(std::abs(centerToCenter.X));
				centerToCenter.Y = static_cast<VecT::DataType>(std::abs(centerToCenter.Y));
				centerToCenter.Z = static_cast<VecT::DataType>(std::abs(centerToCenter.Z));
				centerToCenter.W = static_cast<VecT::DataType>(std::abs(centerToCenter.W));

				VecT ownHalfScale = Scale * 0.5;

				// cant be sure about anything at this point
				ownHalfScale.X = static_cast<VecT::DataType>(std::abs(ownHalfScale.X));
				ownHalfScale.Y = static_cast<VecT::DataType>(std::abs(ownHalfScale.Y));
				ownHalfScale.Z = static_cast<VecT::DataType>(std::abs(ownHalfScale.Z));
				ownHalfScale.W = static_cast<VecT::DataType>(std::abs(ownHalfScale.W));

				VecT otherHalfScale = other.Scale * 0.5;

				otherHalfScale.X = static_cast<VecT::DataType>(std::abs(otherHalfScale.X));
				otherHalfScale.Y = static_cast<VecT::DataType>(std::abs(otherHalfScale.Y));
				otherHalfScale.Z = static_cast<VecT::DataType>(std::abs(otherHalfScale.Z));
				otherHalfScale.W = static_cast<VecT::DataType>(std::abs(otherHalfScale.W));

				VecT halfScaleSum = ownHalfScale + otherHalfScale;

				halfScaleSum.X = static_cast<VecT::DataType>(std::abs(halfScaleSum.X));
				halfScaleSum.Y = static_cast<VecT::DataType>(std::abs(halfScaleSum.Y));
				halfScaleSum.Z = static_cast<VecT::DataType>(std::abs(halfScaleSum.Z));
				halfScaleSum.W = static_cast<VecT::DataType>(std::abs(halfScaleSum.W));

				VecT delta = centerToCenter - halfScaleSum;

				// clamp to overlap
				delta.X = std::min<VecT::DataType>(delta.X, 0);
				delta.Y = std::min<VecT::DataType>(delta.Y, 0);
				delta.Z = std::min<VecT::DataType>(delta.Z, 0);
				delta.W = std::min<VecT::DataType>(delta.W, 0);

				if (delta.X == 0
					|| delta.Y == 0
					|| delta.Z == 0
					|| delta.W == 0)
				{
					// might be touching but no overlap
					return 0;
				}

				const u64 overlap = std::abs(delta.X * delta.Y * delta.Z * delta.W);
				return overlap;
			}

			VecT Position;
			VecT Scale;
		};

		u64 stupidResult = 0;

		std::size_t stupidCheck = acceptedConstraints.size();
		acceptedConstraints.erase(std::unique(acceptedConstraints.begin(), acceptedConstraints.end()), acceptedConstraints.end());
		Assert(acceptedConstraints.size() == stupidCheck);

		Vector<XmasTesseract> acceptedSpaces;
		for (const PartConstraint& constraint : acceptedConstraints)
		{
			stupidResult += constraint.Combinations();
			acceptedSpaces.emplace_back(constraint.MinValues, constraint.MaxValues);
		}

		if (stupidResult > 0)
		{
			return std::to_string(stupidResult);
		}

		Vector<XmasTesseract> rejectedSpaces;
		for (const PartConstraint& constraint : rejectedConstraints)
		{
			const XmasTesseract& space = rejectedSpaces.emplace_back(constraint.MinValues, constraint.MaxValues);
			if constexpr (IsDebug())
			{
				LogFormat("Negative Tesseract (rejected space): {} volume {}", space.ToString(), space.Volume());
			}
		}

		for (u32 i = 0; i < rejectedSpaces.size(); ++i)
		{
			const XmasTesseract& space = rejectedSpaces[i];
			for (u32 j = i + 1; j < rejectedSpaces.size(); ++j)
			{
				const XmasTesseract& otherSpace = rejectedSpaces[j];
				u64 overlap = space.Overlap(otherSpace);
				// assumption: negative spaces do not overlap amongst themselves
				Assert(overlap == 0);
			}
		}

		if constexpr (IsDebug())
		{
			Log("");
		}

		u64 volumeSum = 0;
		for (u32 i = 0; i < acceptedSpaces.size(); ++i)
		{
			const XmasTesseract& space = acceptedSpaces[i];
			u64 volume = space.Volume();

			if constexpr (IsDebug())
			{
				LogFormat("Tesseract {} volume: {} calculating effective volume", space.ToString(), volume);
			}

			for (u32 j = i + 1; j < acceptedSpaces.size(); ++j)
			{
				const XmasTesseract& otherSpace = acceptedSpaces[j];
				u64 overlap = space.Overlap(otherSpace);
				if constexpr (IsDebug())
				{
					LogFormat("Tesseracts\n{}\n{}\noverlap {}", space.ToString(), otherSpace.ToString(), overlap);
				}
				volume -= overlap;
			}

			for (const XmasTesseract& negativeTesseract : rejectedSpaces)
			{
				// negative are not overlapping amongst themselves, we verified that
				u64 overlap = space.Overlap(negativeTesseract);
				if constexpr (IsDebug())
				{
					if (overlap > 0)
					{
						LogFormat("NEGATIVE space overlap: {}", overlap);
					}
				}
				volume -= overlap;
			}

			volumeSum += volume;

			if constexpr (IsDebug())
			{
				LogFormat("Tesseract {} volume: {} effective volume: {}\n", space.ToString(), space.Volume(), volume);
			}
		}

		return std::to_string(volumeSum);
	}
}