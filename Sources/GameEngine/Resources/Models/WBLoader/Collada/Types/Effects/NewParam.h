#pragma once
#include "Sampler.h"
#include "Surface.h"
#include "Types.h"
#include "optional.hpp"

namespace GameEngine
{
namespace Collada
{
struct NewParam
{
    std::string id_;
    wb::optional<Surface> surface_;
    wb::optional<Sampler> sampler_;
};
}
}
