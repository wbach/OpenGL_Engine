#include "MeasurementHandler.h"

namespace Utils
{
MeasurementValue &MeasurementHandler::AddNewMeasurment(const std::string &name)
{
    measurements_.emplace(std::piecewise_construct, std::forward_as_tuple(name), std::forward_as_tuple(""));

    return measurements_.at(name);
}

MeasurementValue &MeasurementHandler::AddNewMeasurment(const std::string &name, const std::string &value)
{
    measurements_.emplace(std::piecewise_construct, std::forward_as_tuple(name), std::forward_as_tuple(value));

    return measurements_.at(name);
}

const std::unordered_map<std::string, MeasurementValue> &MeasurementHandler::GetMeasurments() const
{
    return measurements_;
}
} // namespace Utils
