#include "TimeMeasurer.h"
#include "../GameEngine/Engine/Configuration.h"
#include "Logger/Log.h"
#include "Thread.hpp"
#include <chrono>
#include <thread>

namespace Utils
{
	namespace Time
	{
		CTimeMeasurer::CTimeMeasurer()
			: CTimeMeasurer(static_cast<uint32>(EngineConf.refresRate), EngineConf.vsync)
		{
			Log("Vsync : " + std::to_string(vsync) + ", Refresh rate : " + std::to_string(lockFps));
		}

		CTimeMeasurer::CTimeMeasurer(uint32 lockFps, bool vsync, uint32 frequency)
			: lockFps(lockFps)
			, vsync(vsync)
			, frameCount(0)
			, fps(0)
			, deltaTime(0)
			, deltaTime2(0)
			, frameTime(0)
			, lastFrameTime2(std::chrono::high_resolution_clock::now())
			, lastFrameTime(std::chrono::high_resolution_clock::now())
			, previousTime(std::chrono::high_resolution_clock::now())
			, lockframeTime(frequency / lockFps)
			, frequency_(frequency)
		{

		}

		void CTimeMeasurer::AddOnTickCallback(Callback c)
		{
			callbacks.push_back(c);
		}

		void CTimeMeasurer::CalculateAndLock()
		{
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