#include "ThreadSync.h"

namespace Utils
{
	namespace Thread
	{
		Subscriber::Subscriber(uint32 id, frameFunc func)
			: threadId(id)
			, func(func)
			, isRunning(true)
			, isFree(false)
			, isStarted(false)
		{
		}

		Subscriber::Subscriber(const Subscriber & s)
			: threadId(s.threadId)
			, func(s.func)
			, isRunning(s.isRunning.load())
			, isFree(s.isFree)
			, isStarted(false)
		{
		}

		void Subscriber::Start()
		{
			thread = std::thread(std::bind(&Subscriber::Update, this));
			timeMeasurer.AddOnTickCallback(std::bind(&Subscriber::PrintFps, this));
			isStarted = true;
		}

		void Subscriber::Stop()
		{
			if (!isStarted)
				return;

			isRunning.store(false);
			thread.join();
		}

		void Subscriber::Update()
		{
			while (isRunning.load())
			{
				timeMeasurer.CalculateAndLock();
				float deltaTime = static_cast<float>(timeMeasurer.GetDeltaTime());
				func(deltaTime);
				timeMeasurer.EndFrame();
			}
			Log("Subscriber::Update, End thread.");
		}

		void Subscriber::PrintFps()
		{
			std::string msg = "Thread id : " + std::to_string(threadId) + ", fps : " + std::to_string(timeMeasurer.GetFps()) + ", Frame Time: " + std::to_string(timeMeasurer.GetDeltaTime());
			Log(msg);
		}

		uint32 ThreadSync::Subscribe(frameFunc func)
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

		void ThreadSync::Unsubscribe(uint32 id)
		{
			subscribers[id].isFree = true;
		}

		void ThreadSync::Start()
		{
			for (auto& s : subscribers)
				s.Start();
		}

		void ThreadSync::Stop()
		{
			for (auto& s : subscribers)
				s.Stop();
		}

	} // Thread
} // Utils