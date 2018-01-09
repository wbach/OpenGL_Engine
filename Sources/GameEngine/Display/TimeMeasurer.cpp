#include "TimeMeasurer.h"
#include "../Engine/Configuration.h"
#include "Logger/Log.h"
#include "Thread.hpp"
#include <chrono>
#include <thread>

namespace GameEngine
{
	namespace Time
	{
		CTimeMeasurer::CTimeMeasurer()
			: lockFps(EngineConf.refresRate)
			, vsync(EngineConf.vsync)
			, frameCount(0)
			, fps(0)
			, deltaTime(0)
			, frameTime(0)
			, lockframeTime(1000.0 / lockFps)
		{
			Log("CTimeMeasurer::CTimeMeasurer() Vsync : " + std::to_string(vsync) + ", Refresh rate : " + std::to_string(lockFps));
		}

		void CTimeMeasurer::AddOnTickCallback(Callback c)
		{
			callbacks.push_back(c);
		}

		void CTimeMeasurer::Calculate()
		{
			frameCount++;
			auto time_interval = CalculateFpsTimeInterval();
			CheckFpsTimeElapsed(time_interval / 1000);			
			Lock();
			lastFrameTime = std::chrono::high_resolution_clock::now();
		}

		void CTimeMeasurer::EndFrame()
		{
			deltaTime2 = std::chrono::high_resolution_clock::now() - currentTime;
		}

		float CTimeMeasurer::GetFps() const
		{
			return fps;
		}

		void CTimeMeasurer::RunCallbacks() const
		{
			for (const auto& c : callbacks)
				c();
		}

		int CTimeMeasurer::CalculateFpsTimeInterval()
		{			
			currentTime = std::chrono::high_resolution_clock::now();

			deltaTime = currentTime - lastFrameTime;
			auto d = currentTime - previousTime;

			return static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(d).count());
		}

		void CTimeMeasurer::CheckFpsTimeElapsed(int time_interval)
		{
			if (time_interval < 1)
				return;

			fps = frameCount / (time_interval);
			previousTime = currentTime;
			frameCount = 0;
			RunCallbacks();
		}

		void CTimeMeasurer::Lock()
		{
			if (!vsync)
				return;

			auto currentframeTime = std::chrono::duration_cast<std::chrono::milliseconds>(deltaTime).count();

			if (lockframeTime > currentframeTime)
			{
				auto delay = static_cast<uint32>(lockframeTime - currentframeTime);
				std::this_thread::sleep_for(std::chrono::milliseconds(delay));
			}			
		}

	} // TimeMeasurer
} // GameEngine