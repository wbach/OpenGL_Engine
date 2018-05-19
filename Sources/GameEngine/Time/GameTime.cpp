#include "GameTime.h"

namespace GameEngine
{
	void GameTime::Update(const float& delta_time)
	{
		deltaGameTime_ = delta_time / 1000;
		currentTime_ += deltaGameTime_;

		if (currentTime_ > 1)
			currentTime_ -= 1;
		if (currentTime_ < 0)
			currentTime_ += 1;

		deltaTime_ = delta_time;
	}
}
