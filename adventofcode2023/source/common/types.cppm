export module types;

export import <iterator>;
export import <vector>;
export import <array>;

export import <string>;

export import <numeric>;
export import <limits>;

import assertion;

import <cstdint>;

export namespace aoc
{
	using String = std::string;

	template<typename T>
	using Vector = std::vector<T>;

	template<typename T, size_t Size>
	using Array = std::array<T, Size>;

	using std::ifstream;

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

	//Numeric conversion
	//--------------------------------------------------
	template<typename SignedT, typename UnsignedT>
	UnsignedT SignedToUnsignedImpl(const SignedT& value)
	{
		static_assert(std::numeric_limits<SignedT>::is_signed == true);
		static_assert(std::numeric_limits<UnsignedT>::is_signed == false);
		Assert(value >= 0, "Cannot convert negative value to unsigned");
		return static_cast<UnsignedT>(value);
	}

	u8 ToUnsigned(s8 value) { return SignedToUnsignedImpl<s8, u8>(value); }
	u16 ToUnsigned(s16 value) { return SignedToUnsignedImpl<s16, u16>(value); }
	u32 ToUnsigned(s32 value) { return SignedToUnsignedImpl<s32, u32>(value); }
	u64 ToUnsigned(s64 value) { return SignedToUnsignedImpl<s64, u64>(value); }
}