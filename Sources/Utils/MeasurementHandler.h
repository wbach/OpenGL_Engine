#pragma once
#include <Types.h>

namespace Utils
{
class MeasurementHandler
{
public:
    MeasurementValue& AddNewMeasurment(const std::string&);
    MeasurementValue& AddNewMeasurment(const std::string&, const std::string&);
    const std::unordered_map<std::string, MeasurementValue>& GetMeasurments() const;

private:
    std::unordered_map<std::string, MeasurementValue> measurements_;
};
}  // namespace GameEngine
