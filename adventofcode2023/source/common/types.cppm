export module types;

export import <fstream>;

export import <numeric>;
export import <iterator>;
export import <vector>;
export import <array>;

import <cstdint>;

export namespace aoc
{
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
}