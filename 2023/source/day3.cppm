export module day3;

import common;

export namespace aoc
{
	enum class CellContent
	{
		Invalid,
		Empty,
		Symbol_PotentialGear,
		Symbol_Other,
		Number
	};

	struct NumberInfo
	{
		String ValueString;
		s32 Value = 0;
		u32 RowIndex = 0;
		u32 LocationBeginInclusive = 0;
		u32 LocationEndExclusive = 0;

		u32 Width() const { return LocationEndExclusive - LocationBeginInclusive; }

		bool IsAdjacentToAnySymbol(const Vector<Vector<CellContent>>& cellMap) const
		{
			if constexpr (IsDebug())
			{
				LogFormat("NumberInfo row {} value {} begin {}", RowIndex, Value, LocationBeginInclusive);
			}

			auto debugOutputRow = [&](const Vector<CellContent>& row) {
				if constexpr (IsDebug() == false)
				{
					return;
				}

				String debugRow;
				for (s32 column = LocationBeginInclusive - 1; column <= (s32)LocationEndExclusive; column++)
				{
					if (!InBounds(row, column))
					{
						debugRow.append("|");
						continue;
					}

					switch (row[column])
					{
					case CellContent::Empty: debugRow.append("."); break;
					case CellContent::Symbol_PotentialGear: debugRow.append("G"); break;
					case CellContent::Symbol_Other: debugRow.append("s"); break;
					case CellContent::Number: debugRow.append("#"); break;
					default: Assert(false); break;
					}
				}
				Log(debugRow);
			};

			// Own Row
			const Vector<CellContent>& myRow = cellMap[RowIndex];
			debugOutputRow(myRow);

			if (InBounds(myRow, LocationBeginInclusive - 1) &&
				(myRow[LocationBeginInclusive - 1] == CellContent::Symbol_PotentialGear
					|| myRow[LocationBeginInclusive - 1] == CellContent::Symbol_Other))
			{
				return true;
			}

			if (InBounds(myRow, LocationEndExclusive) &&
				(myRow[LocationEndExclusive] == CellContent::Symbol_PotentialGear
					|| myRow[LocationEndExclusive] == CellContent::Symbol_Other))
			{
				return true;
			}

			// Generalize for adjacent rows
			auto evaluateRow = [&](const Vector<CellContent>& row) -> bool {
				for (s32 column = (s32)LocationBeginInclusive - 1; column <= (s32)LocationEndExclusive; column++)
				{
					if (InBounds(row, column) &&
						(row[column] == CellContent::Symbol_PotentialGear
							|| row[column] == CellContent::Symbol_Other))
					{
						return true;
					}
				}
				return false;
			};

			// Row Up
			if (RowIndex > 0)
			{
				const Vector<CellContent>& upperRow = cellMap[RowIndex - 1];
				debugOutputRow(upperRow);
				if (evaluateRow(upperRow))
				{
					return true;
				}
			}

			// Row Down
			if (cellMap.size() > RowIndex + 1)
			{
				const Vector<CellContent>& bottomRow = cellMap[RowIndex + 1];
				debugOutputRow(bottomRow);
				if (evaluateRow(bottomRow))
				{
					return true;
				}
			}

			return false;
		}

		bool IsAdjacentToLocation(u32 rowIdx, u32 columnIdx) const
		{
			auto dist = [](u32 lhs, u32 rhs) {
				return std::abs(static_cast<s32>(lhs) - static_cast<s32>(rhs));
			};

			if (dist(rowIdx, RowIndex) > 1)
			{
				return false;
			}

			for (u32 i = LocationBeginInclusive; i < LocationEndExclusive; i++)
			{
				if (dist(columnIdx, i) <= 1)
				{
					return true;
				}
			}

			return false;
		}
	};

	void ParseNumber(u32 rowIdx, Vector<CellContent>& row, Vector<NumberInfo>& foundNumbers, u32 contentColumnStart, const String& content)
	{
		const char* contentBegin = content.c_str();
		char* strtolEnd;
		const s32 numberSigned = std::strtol(contentBegin, &strtolEnd, 10);

		Assert(strtolEnd != contentBegin, "no character was interpreted as number");

		NumberInfo numberInfo;
		numberInfo.ValueString = content;
		numberInfo.Value = numberSigned;
		numberInfo.RowIndex = rowIdx;
		numberInfo.LocationBeginInclusive = contentColumnStart;
		numberInfo.LocationEndExclusive = numberInfo.LocationBeginInclusive + NarrowSizeT(numberInfo.ValueString.size());
		Assert(numberInfo.Width() == numberInfo.ValueString.size());

		for (u32 column = numberInfo.LocationBeginInclusive; column < numberInfo.LocationEndExclusive; column++)
		{
			row[column] = CellContent::Number;
		}

		foundNumbers.push_back(std::move(numberInfo));
	}

	void TokenizeAndParse(u32 rowIdx, Vector<CellContent>& row, Vector<NumberInfo>& foundNumbers, u32 contentColumnStart, const String& content)
	{
		Vector<u32> subContentPositions;
		Vector<String> subContents = Split(content, ".", &subContentPositions);
		for (u32 i = 0; i < subContents.size(); i++)
		{
			String& subContent = subContents[i];
			u32 contentPosition = subContentPositions[i] + contentColumnStart;

			Array<char, 10> numberChars;
			std::iota(numberChars.begin(), numberChars.end(), '0');

			size_t symbolLocalPosition = [&]() {
					for (size_t charIdx = 0; charIdx < subContent.size(); charIdx++)
					{
						const char c = subContent[charIdx];

						auto numberIt = std::find(numberChars.begin(), numberChars.end(), c);

						if (numberIt == numberChars.end())
						{
							return charIdx;
						}
					}
					return String::npos;
				}();

			if (symbolLocalPosition == String::npos)
			{
				// Only digits
				ParseNumber(rowIdx, row, foundNumbers, contentPosition, subContent);
				continue;
			}

			u32 symbolPosition = contentPosition + NarrowSizeT(symbolLocalPosition);

			// Manually split at symbol
			{
				// Handle before
				String substringBeforeSymbol = subContent.substr(0, symbolLocalPosition);
				if (substringBeforeSymbol.empty() == false)
				{
					// Directly apply, we should only have digits
					ParseNumber(rowIdx, row, foundNumbers, contentPosition, substringBeforeSymbol);
				}

				// Handle symbol
				const char symbolChar = subContent[symbolLocalPosition];
				if (symbolChar == '*')
				{
					row[symbolPosition] = CellContent::Symbol_PotentialGear;
				}
				else
				{
					row[symbolPosition] = CellContent::Symbol_Other;
				}

				// Handle after
				String substringAfterSymbol = subContent.substr(symbolLocalPosition + 1);
				if (substringAfterSymbol.empty() == false)
				{
					// Recurse, there might be more symbols hitchhiking
					TokenizeAndParse(rowIdx, row, foundNumbers, contentPosition + NarrowSizeT(symbolLocalPosition) + 1, substringAfterSymbol);
				}
			}
		}
	}

	export String ExecutePart1()
	{
		auto input = OpenInput("day3.txt");

		Vector<Vector<CellContent>> rows(140);
		Vector<NumberInfo> foundNumbers;

		u32 currentRowIdx = 0;
		u32 lastRowWidth = 0;
		String line;
		while (std::getline(input, line))
		{
			const u32 currentRowWidth = NarrowSizeT(line.size());
			if (lastRowWidth == 0)
			{
				lastRowWidth = currentRowWidth;
			}
			Assert(currentRowWidth == lastRowWidth);

			Vector<CellContent>& currentRow = rows[currentRowIdx];
			currentRow.assign(currentRowWidth, CellContent::Empty);

			TokenizeAndParse(currentRowIdx, currentRow, foundNumbers, 0, line);

			currentRowIdx++;
		}

		if constexpr (IsDebug())
		{
			for (const Vector<CellContent>& row : rows)
			{
				String debugRow;
				for (CellContent content : row)
				{
					switch (content)
					{
					case CellContent::Empty: debugRow.append("."); break;
					case CellContent::Symbol_PotentialGear: debugRow.append("G"); break;
					case CellContent::Symbol_Other: debugRow.append("s"); break;
					case CellContent::Number: debugRow.append("#"); break;
					default: Assert(false); break;
					}
				}
				Log(debugRow);
			}
		}

		s32 result = 0;
		for (const NumberInfo& numberInfo : foundNumbers)
		{
			const bool numberIsPartNumber = numberInfo.IsAdjacentToAnySymbol(rows);
			if constexpr (IsDebug())
			{
				Log(numberIsPartNumber ? "TRUE" : "FALSE");
			}

			if (numberIsPartNumber)
			{
				result += numberInfo.Value;
			}
		}

		return std::to_string(result);
	}

	export String ExecutePart2()
	{
		auto input = OpenInput("day3.txt");

		Vector<Vector<CellContent>> rows(140);
		Vector<NumberInfo> foundNumbers;

		u32 currentRowIdx = 0;
		u32 lastRowWidth = 0;
		String line;
		while (std::getline(input, line))
		{
			const u32 currentRowWidth = NarrowSizeT(line.size());
			if (lastRowWidth == 0)
			{
				lastRowWidth = currentRowWidth;
			}
			Assert(currentRowWidth == lastRowWidth);

			Vector<CellContent>& currentRow = rows[currentRowIdx];
			currentRow.assign(currentRowWidth, CellContent::Empty);

			TokenizeAndParse(currentRowIdx, currentRow, foundNumbers, 0, line);

			currentRowIdx++;
		}

		if constexpr (IsDebug())
		{
			for (const Vector<CellContent>& row : rows)
			{
				String debugRow;
				for (CellContent content : row)
				{
					switch (content)
					{
					case CellContent::Empty: debugRow.append("."); break;
					case CellContent::Symbol_PotentialGear: debugRow.append("G"); break;
					case CellContent::Symbol_Other: debugRow.append("s"); break;
					case CellContent::Number: debugRow.append("#"); break;
					default: Assert(false); break;
					}
				}
				Log(debugRow);
			}
		}

		u32 result = 0;
		for (u32 rowIdx = 0; rowIdx < rows.size(); rowIdx++)
		{
			const Vector<CellContent>& row = rows[rowIdx];
			for (u32 columnIdx = 0; columnIdx < row.size(); columnIdx++)
			{
				const CellContent cell = row[columnIdx];
				if (cell != CellContent::Symbol_PotentialGear)
				{
					continue;
				}

				Vector<u32> adjacentNumbers;
				for (const NumberInfo& numberInfo : foundNumbers)
				{
					if (numberInfo.IsAdjacentToLocation(rowIdx, columnIdx))
					{
						adjacentNumbers.push_back(numberInfo.Value);
					}
				}

				if (adjacentNumbers.size() != 2)
				{
					continue;
				}

				const u32 gearRatio = adjacentNumbers[0] * adjacentNumbers[1];
				if constexpr (IsDebug())
				{
					LogFormat("Gear row {} column {} has ratio {}", rowIdx, columnIdx, gearRatio);
				}
				result += gearRatio;
			}
		}

		return std::to_string(result);
	}
}