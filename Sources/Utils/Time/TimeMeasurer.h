#pragma once
#include "Types.h"
#include <chrono>
#include <functional>
#include <vector>

namespace Utils
{
	namespace Time
	{
		typedef std::function<void()> Callback;
		typedef std::vector<Callback> Callbacks;

		class CTimeMeasurer
		{
		public:
			CTimeMeasurer();
			CTimeMeasurer(uint32 lockFps, bool vsync = true, uint32 frequency = 1000);
			void AddOnTickCallback(Callback);
			void CalculateAndLock();
			void EndFrame();
			float GetFps() const;
			const double inline GetDeltaTime() const;

		private:
			void RunCallbacks() const;
			uint32  CalculateFpsTimeInterval();
			void CheckFpsTimeElapsed(uint32 time_interval);
			void Lock();

		private:
			uint32 lockFps;
			uint32 frequency_;
			Callbacks callbacks;

			float previousTime_ = 0.f;
			float currentTime_ = 0.f;

			bool vsync;

			Delta deltaTime, deltaTime2;
			Timepoint lastFrameTime, lastFrameTime2;

			Timepoint currentTime, previousTime;
			float frameCount, fps;
			double frameTime;

			const double lockframeTime;
		};

		const double inline CTimeMeasurer::GetDeltaTime() const
		{
			return currentTime_ - previousTime_; //static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(deltaTime2).count()) / 1000000000.0;
		}
	} // TimeMeasurer
} // Utils
