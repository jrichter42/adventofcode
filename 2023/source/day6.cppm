export module day6;

import common;

export namespace aoc
{
	export String ExecutePart1()
	{
		auto input = OpenInput("day6.txt");

		String line;

		std::getline(input, line);

		Vector<String> timeInfo = Split(line, ":");
		Assert(timeInfo[0] == "Time");
		Vector<String> timeStrings = Split(timeInfo[1], " ");

		std::getline(input, line);

		Vector<String> distanceInfo = Split(line, ":");
		Assert(distanceInfo[0] == "Distance");
		Vector<String> distanceStrings = Split(distanceInfo[1], " ");

		Assert(timeStrings.size() == distanceStrings.size());

		u32 result = 1;
		for (u32 raceIndex = 0; raceIndex < timeStrings.size(); raceIndex++)
		{
			const String& timeString = timeStrings[raceIndex];
			const String& distanceString = distanceStrings[raceIndex];
			const u32 time = std::stoi(timeString);
			const u32 distance = std::stoi(distanceString);

			u32 failAttempts = 0;
			u32 successAttempts = 0;
			for (u32 msUntilRelease = 0; msUntilRelease <= time; msUntilRelease++)
			{
				const u32 msAfterRelease = time - msUntilRelease;
				const u32 speedAfterRelease = msUntilRelease;
				const u32 attemptDistance = msAfterRelease * speedAfterRelease;

				if (attemptDistance > distance)
				{
					successAttempts++;
				}
				else
				{
					failAttempts++;
				}
			}

			const u32 raceMarginOfError = successAttempts;
			result *= raceMarginOfError;
		}

		return std::to_string(result);
	}

	export String ExecutePart2()
	{
		auto input = OpenInput("day6.txt");

		String line;

		std::getline(input, line);

		Vector<String> timeInfo = Split(line, ":");
		Assert(timeInfo[0] == "Time");
		String& timeString = timeInfo[1];
		timeString.erase(std::remove(timeString.begin(), timeString.end(), ' '), timeString.end());

		std::getline(input, line);

		Vector<String> distanceInfo = Split(line, ":");
		Assert(distanceInfo[0] == "Distance");
		String& distanceString = distanceInfo[1];
		distanceString.erase(std::remove(distanceString.begin(), distanceString.end(), ' '), distanceString.end());

		const u64 time = std::stoll(timeString);
		const u64 distance = std::stoll(distanceString);

		u32 successAttempts = 0;
		for (u64 msUntilRelease = 0; msUntilRelease <= time; msUntilRelease++)
		{
			const u64 msAfterRelease = time - msUntilRelease;
			const u64 speedAfterRelease = msUntilRelease;
			const u64 attemptDistance = msAfterRelease * speedAfterRelease;

			if (attemptDistance > distance)
			{
				successAttempts++;
			}
		}

		const u32 raceMarginOfError = successAttempts;

		return std::to_string(raceMarginOfError);
	}
}