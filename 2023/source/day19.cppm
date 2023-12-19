export module day19;

import common;

export namespace aoc
{
	struct Part
	{
		u64 GetRating() const
		{
			return x + m + a + s;
		}

		using MemberT = u32;
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
		{}

		PartMemberSelector PartMember = PartMemberSelector::Invalid;
		Compare Comparator = std::less<Part::MemberT>();
		Part::MemberT ValueToCompareTo;
		String WorkflowOnSuccess;

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
							  rule.Comparator(1, 0) ? ">" : "<",
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
						Part::MemberT valueToCompareTo = std::stoi(ruleExpression[1]);
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

					Part::MemberT value = std::stoi(memberValueStrings[1]);
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
		return "";
	}
}