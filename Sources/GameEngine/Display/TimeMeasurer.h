#pragma once
#include "Types.h"
#include <chrono>
#include <functional>
#include <vector>

namespace GameEngine
{
	namespace Time
	{
		typedef std::function<void()> Callback;
		typedef std::vector<Callback> Callbacks;

		class CTimeMeasurer
		{
		public:
			CTimeMeasurer();
			void AddOnTickCallback(Callback);
			void Calculate();
			void EndFrame();
			float GetFps() const;
			const double inline GetDeltaTime() const;

		private:
			void RunCallbacks() const;
			int  CalculateFpsTimeInterval();
			void CheckFpsTimeElapsed(int time_interval);
			void Lock();

		private:
			uint32 lockFps;
			Callbacks callbacks;

			bool vsync;

			Delta deltaTime, deltaTime2;
			Timepoint lastFrameTime;

			Timepoint currentTime, previousTime;
			float frameCount, fps;
			double frameTime;

			const double lockframeTime;
		};

		const double inline CTimeMeasurer::GetDeltaTime() const
		{
			return static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(deltaTime2).count()) / 1000000000.0;
		}
	} // TimeMeasurer
} // GameEngine
