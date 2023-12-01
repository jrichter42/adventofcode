module;

#pragma warning(push, 0)
//#include <Windows.h>
#pragma warning(pop)

export module common;

export import <string>;
export using std::string;

export import <fstream>;
export using std::ifstream;

import <iostream>;

export namespace aoc
{
	void Log(const string& message)
	{
		std::cout << message << std::endl;
		//OutputDebugString(result.c_str());
	}

	[[nodiscard]] ifstream Open(const string& filepath)
	{
        ifstream result(filepath);
        if (!result.is_open())
        {
            Log("Error: Couldn't open file");
            Log(filepath);
        }

        return result;

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