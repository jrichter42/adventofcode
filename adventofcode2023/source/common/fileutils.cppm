export module fileutils;

import types;
import logging;

export import <fstream>; // std::ifstream

export namespace aoc
{
	[[nodiscard]] ifstream Open(const String& filepath)
	{
		ifstream result(filepath);
		if (!result.is_open())
		{
			Log("Error: Couldn't open file");
			Log(filepath);
		}

		return result;

	}

	[[nodiscard]] ifstream OpenInput(const String& filepathRelativeToInput)
	{
		return Open(String("input/") + filepathRelativeToInput);
	}

	template<typename DataT>
	bool Read(ifstream& filestream, DataT& data)
	{
		filestream >> data;

		if (filestream.fail())
		{
			Log("Error: Failed to read from filestream");

			switch (filestream.rdstate())
			{
			case ifstream::badbit:
			{
				Log("badbit: irrecoverable stream error");
			} break;
			case ifstream::failbit:
			{
				Log("failbit: input/output operation failed (formatting or extraction error)");
			} break;
			case ifstream::eofbit:
			{
				Log("eofbit: associated input sequence has reached end-of-file");
			} break;
			}

			return false;
		}

		return true;
	}

	template<typename DataT>
	[[nodiscard]] DataT Read(ifstream& filestream)
	{
		DataT result;
		Read(filestream, result);
		return result;
	}
}