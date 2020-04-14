#pragma once
#include <Types.h>
#include <Utils/ThreadSync.h>
#include <unordered_map>
#include "SingleTon.h"

namespace GameEngine
{
class EngineContextData
{
public:
    EngineContextData();
    ~EngineContextData();
    MeasurementValue &AddNewMeasurment(const std::string&);
    MeasurementValue &AddNewMeasurment(const std::string&, const std::string&);
    const std::unordered_map<std::string, MeasurementValue>& GetMeasurments() const;

private:
    std::unordered_map<std::string, MeasurementValue> measurements_;

public:
    Utils::Thread::ThreadSync threadSync_;
};
}  // namespace GameEngine
#define EngineContext SingleTon<GameEngine::EngineContextData>::Get()
