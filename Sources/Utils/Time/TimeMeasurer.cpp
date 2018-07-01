#include "TimeMeasurer.h"
#include "../GameEngine/Engine/Configuration.h"
#include "Logger/Log.h"
#include "Thread.hpp"
#include <chrono>
#include <thread>
#include <iostream>

float GetCTime()
{
	return static_cast<float>(clock()) / static_cast<float>(CLOCKS_PER_SEC);
}

namespace Utils
{
	namespace Time
	{
		CTimeMeasurer::CTimeMeasurer()
			: CTimeMeasurer(static_cast<uint32>(EngineConf.renderer.fpsLimt))
		{
			Log("Vsync : " + std::to_string(vsync) + ", Refresh rate : " + std::to_string(lockFps));
		}

		CTimeMeasurer::CTimeMeasurer(uint32 lockFps, uint32 frequency)
			: lockFps(lockFps)
			, frequency_(frequency)
			, previousTime_(0.f)
			, currentTime_(0.f)
			, vsync(lockFps > 0)
			, deltaTime(0)
			, deltaTime2(0)
			, lastFrameTime(std::chrono::high_resolution_clock::now())
			, lastFrameTime2(std::chrono::high_resolution_clock::now())
			, currentTime(std::chrono::high_resolution_clock::now())
			, previousTime(std::chrono::high_resolution_clock::now())
			, frameCount(0)
			, fps(0)
			, frameTime(0)
			, lockframeTime(frequency / lockFps)
		{

		}

		void CTimeMeasurer::AddOnTickCallback(Callback c)
		{
			callbacks.push_back(c);
		}

		void CTimeMeasurer::CalculateAndLock()
		{
			previousTime_ = currentTime_;
			currentTime_ = GetCTime();
			//std::cout << GetDeltaTime() << std::endl;

			currentTime = std::chrono::high_resolution_clock::now();
			frameCount++;

			auto time_interval = CalculateFpsTimeInterval();
			CheckFpsTimeElapsed(time_interval);

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

		uint32 CTimeMeasurer::CalculateFpsTimeInterval()
		{	
			deltaTime = currentTime - lastFrameTime;

			auto d = currentTime - previousTime;

			return static_cast<uint32>(std::chrono::duration_cast<std::chrono::milliseconds>(d).count());
		}

		void CTimeMeasurer::CheckFpsTimeElapsed(uint32 time_interval)
		{
			if (time_interval < frequency_)
				return;

			float time_interval_ms = static_cast<float>(time_interval) / static_cast<float>(frequency_);
			fps = frameCount / time_interval_ms;
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
} // Utils