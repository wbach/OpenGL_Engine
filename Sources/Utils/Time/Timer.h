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
		std::chrono::time_point<std::chrono::steady_clock> start_;
	};
} // Utils
