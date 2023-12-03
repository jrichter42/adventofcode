export module day3;

import common;

export namespace aoc
{
	enum class CellContent
	{
		Invalid,
		Empty,
		Symbol,
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

		bool IsAdjacentToSymbol(const Vector<Vector<CellContent>>& cellMap) const
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
					case CellContent::Symbol: debugRow.append("s"); break;
					case CellContent::Number: debugRow.append("#"); break;
					default: Assert(false); break;
					}
				}
				//Assert(debugRow != "|...s");
				Log(debugRow);
			};

			// Own Row
			const Vector<CellContent>& myRow = cellMap[RowIndex];
			debugOutputRow(myRow);

			if (InBounds(myRow, LocationBeginInclusive - 1) && myRow[LocationBeginInclusive - 1] == CellContent::Symbol)
			{
				return true;
			}

			if (InBounds(myRow, LocationEndExclusive) && myRow[LocationEndExclusive] == CellContent::Symbol)
			{
				return true;
			}

			// Generalize for adjacent rows
			auto evaluateRow = [&](const Vector<CellContent>& row) -> bool {
				for (s32 column = (s32)LocationBeginInclusive - 1; column <= (s32)LocationEndExclusive; column++)
				{
					if (InBounds(row, column) && row[column] == CellContent::Symbol)
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
	};

	void ParseNumber(u32 rowIdx, Vector<CellContent>& row, Vector<NumberInfo>& foundNumbers, u32 contentColumnStart, const String& content)
	{
		const char* contentBegin = content.c_str();
		char* strtolEnd;
		const s32 numberSigned = strtol(contentBegin, &strtolEnd, 10);

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
				row[symbolPosition] = CellContent::Symbol;

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
					case CellContent::Symbol: debugRow.append("s"); break;
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
			const bool numberIsPartNumber = numberInfo.IsAdjacentToSymbol(rows);
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
		return "";
	}
}