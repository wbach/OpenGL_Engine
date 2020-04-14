#pragma once
#include <Utils/ThreadSync.h>
#include <unordered_map>
#include "SingleTon.h"

namespace GameEngine
{
struct EngineContextData
{
    ~EngineContextData();
    Utils::Thread::ThreadSync threadSync_;
    std::unordered_map<std::string, std::string> measurements_;
};
}  // namespace GameEngine
#define EngineContext SingleTon<GameEngine::EngineContextData>::Get()
