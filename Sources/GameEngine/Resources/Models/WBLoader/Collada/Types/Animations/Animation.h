#pragma once
#include <unordered_map>
#include "../Common/Source.h"
#include "AnimationSampler.h"
#include "Channel.h"
#include "Types.h"

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
