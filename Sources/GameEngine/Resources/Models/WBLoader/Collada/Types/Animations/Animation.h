#pragma once
#include "Types.h"
#include "../Common/Source.h"
#include "AnimationSampler.h"
#include "Channel.h"
#include <unordered_map>

namespace GameEngine
{
	namespace Collada
	{
		struct Animation
		{
			std::string id_;
			Channel channel_;
			AnimationSampler sampler_;
			std::unordered_map<std::string, Source> sources_;
		};
	}
}
