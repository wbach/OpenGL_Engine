#pragma once
#include "SingleTon.h"
#include <unordered_map>

namespace GameEngine
{
struct EngineContextData
{
    std::unordered_map<std::string, std::string> measurements_;
};
}  // namespace GameEngine
#define EngineContext SingleTon<GameEngine::EngineContextData>::Get()
