module;

#pragma warning(push, 0)
#include <Windows.h>
#pragma warning(pop)

#include <cassert>

export module common;

export import <cstring>;
export import <string>;
export import <format>;
export import <fstream>;

export import <algorithm>;
export import <numeric>;

export import <iterator>;
export import <vector>;
export import <array>;


import <iostream>;
import <cstdint>;

export namespace aoc
{
#pragma region Types
	export using String = std::string;

	export using std::format;
	export using std::ifstream;
	export using std::vector;
	export using std::array;

	//Numeric types
	//--------------------------------------------------
	using s8 = int8_t;
	using u8 = uint8_t;

	using s16 = int16_t;
	using u16 = uint16_t;

	using s32 = int32_t;
	using u32 = uint32_t;

	using s64 = int64_t;
	using u64 = uint64_t;

	using f32 = float;
	using f64 = double;

	using byte = unsigned char;
	//using uint = unsigned int;

	//Numeric limits
	//--------------------------------------------------
	constexpr const s8 s8_MAX = INT8_MAX;
	constexpr const s8 s8_MIN = INT8_MIN;
	constexpr const u8 u8_MAX = UINT8_MAX;

	constexpr const s16 s16_MAX = INT16_MAX;
	constexpr const s16 s16_MIN = INT16_MIN;
	constexpr const u16 u16_MAX = UINT16_MAX;

	constexpr const s32 s32_MAX = INT32_MAX;
	constexpr const s32 s32_MIN = INT32_MIN;
	constexpr const u32 u32_MAX = UINT32_MAX;

	constexpr const s64 s64_MAX = INT64_MAX;
	constexpr const s64 s64_MIN = INT64_MIN;
	constexpr const u64 u64_MAX = UINT64_MAX;

	constexpr const f32 f32_MAX = FLT_MAX;
	constexpr const f32 f32_MIN = FLT_MIN;
	constexpr const f64 f64_MAX = DBL_MAX;
	constexpr const f64 f64_MIN = DBL_MIN;
#pragma endregion Types

	template<typename TCondition>
	auto constexpr Assert(TCondition&& condition)
	{
		if (std::forward<TCondition>(condition) == false)
		{
			__debugbreak();
		}
		//return assert(std::forward<TCondition>(condition));
	}

	template<typename TCondition>
	auto constexpr Assert(TCondition&& condition, auto message)
	{
		if (std::forward<TCondition>(condition) == false)
		{
			__debugbreak();
		}
		//return assert(std::forward<TCondition>(condition));
	}

	void Log(const String& message)
	{
		std::cout << message << std::endl;
		OutputDebugStringA(message.c_str());
		OutputDebugStringA("\n");
	}

	template<typename... Args>
	void LogFormat(std::format_string<Args...> messageFormat, Args&&... args)
	{
		using FormatStringT = std::format_string<Args...>;
		String formattedMessage = format(std::forward<FormatStringT>(messageFormat), std::forward<Args>(args)...);
		Log(formattedMessage);
	}

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