export module common;

export import blib;

export namespace aoc
{
	using namespace blib;

	[[nodiscard]] ifstream OpenInput(const String& filepathRelativeToInput)
	{
		return Open(String("input/") + filepathRelativeToInput);
	}
}