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

	export String ExecutePart2()
	{
		return "";
	}
}