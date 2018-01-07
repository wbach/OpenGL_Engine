#pragma once
#include "../Api/ApiMessages.h"
#include "Thread.hpp"
#include "../Display/TimeMeasurer.h"
#include "Types.h"
#include "Logger/Log.h"
#include <vector>
#include <atomic>

#include <iostream>
namespace GameEngine
{
	namespace Thread
	{
		typedef std::function<ApiMessages::Type(float deltaTime)> frameFunc;

		struct Subscriber
		{
			Subscriber(uint32 id, frameFunc func)
				: threadId(id)
				, func(func)
				, isRunning(true)
				, isFree(false)
			{
			}

			Subscriber(const Subscriber& s)
				: threadId(s.threadId)
				, func(s.func)
				, isRunning(s.isRunning.load())
				, isFree(s.isFree)
			{
			}

			~Subscriber()
			{
				Log(__FUNCTION__);
			}

			void Start()
			{
				thread = std::thread(std::bind(&Subscriber::Update, this));
				timeMeasurer.AddOnTickCallback(std::bind(&Subscriber::PrintFps, this));
			}

			void Update()
			{
				while (isRunning.load())
				{
					timeMeasurer.Calculate();				

					float deltaTime = static_cast<float>(timeMeasurer.GetDeltaTime());
					//std::cout << "deltaTime : " << deltaTime << std::endl;					

					switch (func(deltaTime))
					{
					case 1: isRunning.store(false); break;
					}

					timeMeasurer.EndFrame();

					//Log(__FUNCTION__);
					//std::this_thread::sleep_for(std::chrono::milliseconds(50));
				}
			}

			void PrintFps()
			{
				std::string msg = "Thread id : " + std::to_string(threadId) + ", fps : " + std::to_string(timeMeasurer.GetFps()) + ", Frame Time: " + std::to_string(timeMeasurer.GetDeltaTime());
				Log(msg);
			}

			void Stop()
			{
				isRunning.store(false);
				thread.join();
			}

			Time::CTimeMeasurer timeMeasurer;
			frameFunc func;
			std::thread thread;
			std::atomic_bool isRunning;
			bool isFree;
			uint32 threadId;
		};

		class ThreadSync
		{
		public:
			~ThreadSync()
			{
				Log(__FUNCTION__);
			}

			uint32 Subscribe(frameFunc func)
			{
				uint32 i = 0;
				for (auto& s : subscribers)
				{
					if (s.isFree)
					{
						subscribers[i].threadId = i;
						subscribers[i].func = func;
						subscribers[i].isFree = false;
						return i;
					}
					++i;
				}

				subscribers.emplace_back(i, func);
				return subscribers.size() - 1;
			}

			void Unsubscribe(uint32 id)
			{
				subscribers[id].isFree = true;
			}

			void Start()
			{
				for (auto& s : subscribers)
					s.Start();
			}
			void Stop()
			{
				for (auto& s : subscribers)
					s.Stop();
			}

		private:
			std::vector<Subscriber> subscribers;
		};
	} // Thread
} // GameEngine