export module day8;

import common;

export namespace aoc
{
	enum class Direction
	{
		Invalid,
		Left,
		Right
	};

	struct Node
	{
		String ID;
		String LinkLeftNodeID;
		String LinkRightNodeID;

		bool operator<(Node& rhs) const
		{
			for (u32 i = 0; i < 3; i++)
			{
				if (ID[i] != rhs.ID[i])
				{
					return ID[i] < rhs.ID[i];
				}
			}
			return false;
		}

		bool operator==(Node& rhs) const
		{
			return ID == rhs.ID;
		}
	};

	export String ExecutePart1()
	{
		auto input = OpenInput("day8.txt");

		String line;
		std::getline(input, line);

		Vector<Direction> directions;
		for (char lrC : line)
		{
			if (lrC == 'L')
			{
				directions.push_back(Direction::Left);
				continue;
			}
			Assert(lrC == 'R');
			directions.push_back(Direction::Right);
		}
		Assert(directions.size() == line.size());

		std::getline(input, line);
		Assert(line.empty());

		std::map<String, Node> nodes;
		while (std::getline(input, line))
		{
			// Parse vertices and edges
			Vector<String> nodeLinksString = Split(line, " = ");
			Assert(nodeLinksString.size() == 2);
			const String& nodeString = nodeLinksString[0];
			const String& linksString = nodeLinksString[1];

			Node node;
			node.ID = nodeString;

			String linkStringWithoutParen = Split(Split(linksString, "(")[0], ")")[0]; // oh well
			Vector<String> linkStrings = Split(linkStringWithoutParen, ", ");
			Assert(linkStrings.size() == 2);
			const String& linkLeftStr = linkStrings[0];
			const String& linkRightStr = linkStrings[1];

			node.LinkLeftNodeID = linkLeftStr;
			node.LinkRightNodeID = linkRightStr;

			Assert(nodes.contains(node.ID) == false);
			nodes[node.ID] = std::move(node);
		}

		Node& currentNode = nodes["AAA"];
		u32 stepCount = 0;
		while (currentNode.ID != "ZZZ")
		{
			for (const Direction dir : directions)
			{
				Assert(dir != Direction::Invalid);

				if (dir == Direction::Left)
				{
					currentNode = nodes[currentNode.LinkLeftNodeID];
				}
				else
				{
					currentNode = nodes[currentNode.LinkRightNodeID];
				}

				stepCount++;

				if (currentNode.ID == "ZZZ")
				{
					break;
				}
			}
		}

		return std::to_string(stepCount);
	}

	// https://en.cppreference.com/w/cpp/numeric/lcm
	constexpr auto lcm(auto x, auto... xs)
	{
		return ((x = std::lcm(x, xs)), ...);
	}

	// https://stackoverflow.com/questions/58633925/expand-stdvector-into-parameter-pack
	template <std::size_t ... Indices>
	u64 lcmFromVec(const std::vector<u64>& elements, const std::index_sequence<Indices...>&)
	{
		return lcm(elements[Indices]...);
	}

	template <std::size_t N>
	u64 lcmFromVec(const std::vector<u64>& elements)
	{
		return lcmFromVec(elements, std::make_index_sequence<N>{});
	}

	export String ExecutePart2()
	{
		auto input = OpenInput("day8.txt");

		String line;
		std::getline(input, line);

		Vector<Direction> directions;
		for (char lrC : line)
		{
			if (lrC == 'L')
			{
				directions.push_back(Direction::Left);
				continue;
			}
			Assert(lrC == 'R');
			directions.push_back(Direction::Right);
		}
		Assert(directions.size() == line.size());

		std::getline(input, line);
		Assert(line.empty());

		std::unordered_map<String, Node> nodes;
		Vector<String> startNodeIDs;
		Vector<String> endNodeIDs;

		while (std::getline(input, line))
		{
			// Parse vertices and edges
			Vector<String> nodeLinksString = Split(line, " = ");
			Assert(nodeLinksString.size() == 2);
			const String& nodeString = nodeLinksString[0];
			const String& linksString = nodeLinksString[1];

			if (nodeString[2] == 'A')
			{
				startNodeIDs.push_back(nodeString);
			}
			else if (nodeString[2] == 'Z')
			{
				endNodeIDs.push_back(nodeString);
			}

			Node node;
			node.ID = nodeString;

			String linkStringWithoutParen = Split(Split(linksString, "(")[0], ")")[0]; // oh well
			Vector<String> linkStrings = Split(linkStringWithoutParen, ", ");
			Assert(linkStrings.size() == 2);
			const String& linkLeftStr = linkStrings[0];
			const String& linkRightStr = linkStrings[1];

			node.LinkLeftNodeID = linkLeftStr;
			node.LinkRightNodeID = linkRightStr;

			Assert(nodes.contains(node.ID) == false);
			nodes[node.ID] = std::move(node);
		}

		Vector<Node> currentNodes;
		for (const String& startNodeID : startNodeIDs)
		{
			currentNodes.push_back(nodes[startNodeID]);
		}

		/*auto reachedEnd = [&]() -> bool {
			for (const Node& currentNode : currentNodes)
			{
				if (currentNode.ID[2] != 'Z')
				{
					return false;
				}
			}
			return true;
		};*/

		Vector<u64> stepsUntilEnd;

		for (Node& currentNode : currentNodes)
		{
			u64 stepCount = 0;
			bool endReached = false;
			while (endReached == false)
			{
				for (const Direction dir : directions)
				{
					//Assert(dir != Direction::Invalid);

					if (dir == Direction::Left)
					{
						currentNode = nodes[currentNode.LinkLeftNodeID];
					}
					else
					{
						currentNode = nodes[currentNode.LinkRightNodeID];
					}

					stepCount++;

					if (currentNode.ID[2] == 'Z')
					{
						endReached = true;
						break;
					}
				}
			}
			stepsUntilEnd.push_back(stepCount);
		}

		Assert(stepsUntilEnd.size() == 6);
		const u64 totalStepsNeeded = lcmFromVec<6>(stepsUntilEnd);

		return std::to_string(totalStepsNeeded);
	}
}