#pragma once
#include <math.h>

namespace GameEngine
{
	class GameTime
	{
	public:
		GameTime()
			: currentTime_(0.12f)
			, deltaTime_(0.f)
			, deltaGameTime_(0.f)
		{

		}
		~GameTime() {}
		virtual void Update(const float& delta_time);
		float GetHours() const;
		float GetMinutes() const;
		float GetSeconds() const;
		float GetTime() const;
		float GetDeltaTime() const;
		float GetDeltaGameTime() const;
		void SetTime(const float& time);

	protected:
		float currentTime_;
		float deltaTime_;
		float deltaGameTime_;
	};

	inline float GameTime::GetHours() const
	{
		return currentTime_ * 24.f;
	}
	inline float GameTime::GetMinutes() const
	{
		return currentTime_ * 24.f * 60.f;
	}
	inline float GameTime::GetSeconds() const
	{
		return currentTime_ * 24.f * 60.f * 60.f;
	}
	inline float GameTime::GetTime() const
	{
		return currentTime_;
	}
	inline float GameTime::GetDeltaTime() const
	{
		return deltaTime_;
	}
	inline float GameTime::GetDeltaGameTime() const
	{
		return deltaGameTime_;
	}
	inline void GameTime::SetTime(const float& time)
	{
		currentTime_ = time;
	}
}// GameEngine
