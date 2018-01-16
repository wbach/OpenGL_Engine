#pragma once
#include "../GameEngine/Api/ApiMessages.h"
#include "Time/TimeMeasurer.h"
#include "Thread.hpp"
#include "Types.h"
#include "Logger/Log.h"
#include <vector>
#include <atomic>
#include <iostream>

namespace Utils
{
	namespace Thread
	{
		typedef std::function<ApiMessages::Type(float deltaTime)> frameFunc;

		struct Subscriber
		{
			Subscriber(uint32 id, frameFunc func);
			Subscriber(const Subscriber& s);

			void Start();
			void Stop();
			void Update();
			void PrintFps();

			bool isFree;
			frameFunc func;
			uint32 threadId;
			std::thread thread;
			std::atomic_bool isRunning;
			Time::CTimeMeasurer timeMeasurer;
		};

		class ThreadSync
		{
		public:
			uint32 Subscribe(frameFunc func);
			void Unsubscribe(uint32 id);
			void Start();
			void Stop();

		private:
			std::vector<Subscriber> subscribers;
		};
	} // Thread
} // Utils