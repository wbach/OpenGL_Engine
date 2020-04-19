#include "EngineContext.h"

namespace GameEngine
{
EngineContextData::EngineContextData()
    : threadSync_([this](const std::string &name) -> MeasurementValue & { return AddNewMeasurment(name); })
{
}

EngineContextData::~EngineContextData()
{
    DEBUG_LOG("destructor");
    threadSync_.Stop();
}

MeasurementValue &EngineContextData::AddNewMeasurment(const std::string &name)
{
    measurements_.emplace(std::piecewise_construct, std::forward_as_tuple(name), std::forward_as_tuple(""));

    return measurements_.at(name);
}

MeasurementValue &EngineContextData::AddNewMeasurment(const std::string &name, const std::string &value)
{
    measurements_.emplace(std::piecewise_construct, std::forward_as_tuple(name), std::forward_as_tuple(value));

    return measurements_.at(name);
}

const std::unordered_map<std::string, MeasurementValue> &EngineContextData::GetMeasurments() const
{
    return measurements_;
}
}  // namespace GameEngine
