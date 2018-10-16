#pragma once
#include "Types.h"
#include <chrono>

namespace Utils
{
	class Timer
	{
	public:
		Timer();
		uint64 GetTimeMiliseconds();

	private:
		Timepoint start_;
	};
} // Utils
